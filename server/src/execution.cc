#include <iostream>

#include "execution.h"

namespace map_reduce
{

void Master::interrupt()
{
  interrupted.store(true);
}

ErrorCode Master::execute_task(const ClientFunctions &cl_funcs, const char *client_name, size_t num_threads)
{
  init_task(cl_funcs, client_name, num_threads);
  print_task_begin();
  read_phase();
  map_phase();

  if (interrupted.load() == true)
  {
    // Fake reduce, so that workers can finish
    fake_reduce();
    term_task();
    print_interrupt();
    return ErrorCode::TASK_INTERRUPTED;
  }

  shuffle_phase();

  if (interrupted.load() == true)
  {
    // Fake reduce, so that workers can finish
    fake_reduce();
    term_task();
    print_interrupt();
    return ErrorCode::TASK_INTERRUPTED;
  }

  reduce_phase();

  if (interrupted.load() == true)
  {
    term_task();
    print_interrupt();
    return ErrorCode::TASK_INTERRUPTED;
  }

  write_phase();
  print_task_finish();
  term_task();

  return ErrorCode::OK;
}

void Master::fake_reduce()
{
  reduce_inputs_iter = reduce_inputs.cend();
  {
    std::lock_guard<std::mutex> lock(event_worker_reduce_mutex);
    current_worker_task = WorkerTask::REDUCE;
  }
  event_worker_reduce.notify_all();
}

void Master::init_task(const ClientFunctions &cl_funcs, const char *client_name, size_t num_threads)
{
  size_t max_threads = std::thread::hardware_concurrency();
  if (num_threads > max_threads)
    num_threads = max_threads;

  task_funcs.read = (ReadFunction)cl_funcs.read;
  task_funcs.write = (WriteFunction)cl_funcs.write;
  task_funcs.map = (MapFunction)cl_funcs.map;
  task_funcs.reduce = (ReduceFunction)cl_funcs.reduce;

  {
    std::lock_guard<std::mutex> lock(task_info_mutex);
    task_info.client_name = client_name;
    task_info.num_threads = num_threads;
    task_info.task_begin = std::chrono::steady_clock::now();
    task_info.is_active = true;
  }

  current_worker_task = WorkerTask::REST;

  for (size_t i = 0; i < num_threads; ++i)
    workers.emplace_back(&Master::worker_routine, this);

  interrupted.store(false);
}

void Master::term_task()
{
  for (auto& worker : workers)
    worker.join();

  workers.clear();
  map_inputs.clear();
  map_outputs.clear();
  reduce_inputs.clear();
  reduce_outputs.clear();

  {
    std::lock_guard<std::mutex> lock(task_info_mutex);
    task_info.is_active = false;
  }
}

void Master::read_phase()
{
  map_inputs = std::move(task_funcs.read());
}

void Master::map_phase()
{
  map_inputs_index = 0;
  workers_done_count = 0;
  {
    std::lock_guard<std::mutex> lock(event_worker_map_mutex);
    current_worker_task = WorkerTask::MAP;
  }
  event_worker_map.notify_all();

  {
    std::unique_lock<std::mutex> lock(workers_done_mutex);
    event_workers_done.wait(lock, [this] { return workers_done_count == workers.size(); });
  }
}

void Master::shuffle_phase()
{
  for (auto &vec : map_outputs)
    for (auto &pair : vec)
      reduce_inputs[pair.first].push_back(pair.second);
}

void Master::reduce_phase()
{
  reduce_inputs_iter = reduce_inputs.cbegin();
  workers_done_count = 0;
  {
    std::lock_guard<std::mutex> lock(event_worker_reduce_mutex);
    current_worker_task = WorkerTask::REDUCE;
  }
  event_worker_reduce.notify_all();

  {
    std::unique_lock<std::mutex> lock(workers_done_mutex);
    event_workers_done.wait(lock, [this] { return workers_done_count == workers.size(); });
  }
}

void Master::write_phase()
{
  task_funcs.write(reduce_outputs);
}

void Master::worker_routine()
{
  {
    std::unique_lock<std::mutex> lock(event_worker_map_mutex);
    event_worker_map.wait(lock, [this] { return current_worker_task == WorkerTask::MAP; });
  }

  worker_map();
  increase_done_count();

  {
    std::unique_lock<std::mutex> lock(event_worker_reduce_mutex);
    event_worker_reduce.wait(lock, [this] { return current_worker_task == WorkerTask::REDUCE; });
  }

  worker_reduce();
  increase_done_count();
}

void Master::increase_done_count()
{
  bool notify_master = false;

  {
    std::lock_guard<std::mutex> lock(workers_done_mutex);
    ++workers_done_count;

    if (workers_done_count == workers.size())
      notify_master = true;
  }

  if (notify_master)
    event_workers_done.notify_one();
}

void Master::worker_map()
{
  std::pair<KeyIn, ValueIn> current_input;

  while (true)
  {
    {
      std::lock_guard<std::mutex> lock(worker_inputs_mutex);
      if (map_inputs_index == map_inputs.size())
        break;

      current_input = map_inputs[map_inputs_index];
      ++map_inputs_index;
    }

    auto current_output = std::move(task_funcs.map(current_input.first, current_input.second));

    {
      std::lock_guard<std::mutex> lock(worker_outputs_mutex);
      map_outputs.push_back(std::move(current_output));
    }
  }
}

void Master::worker_reduce()
{
  std::pair<KeyOut, std::vector<ValueOut>> current_input;

  while (true)
  {
    {
      std::lock_guard<std::mutex> lock(worker_inputs_mutex);
      if (reduce_inputs_iter == reduce_inputs.cend())
        break;

      current_input = *reduce_inputs_iter;
      ++reduce_inputs_iter;
    }

    auto current_output = std::move(task_funcs.reduce(current_input.first, current_input.second));

    {
      std::lock_guard<std::mutex> lock(worker_outputs_mutex);
      reduce_outputs.push_back(std::move(current_output));
    }
  }
}

void Master::print_task_info()
{
  std::lock_guard<std::mutex> lock(task_info_mutex);
  if (task_info.is_active)
  {
    std::cout << "[MASTER] Client: \'" << task_info.client_name << "\'"
            << "\n         Number of threads: " << task_info.num_threads
            << "\n         Running for: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - task_info.task_begin).count()
            << "ms\n";
  }
  else
    std::cout << "[MASTER] There is no active task.\n";
}

void Master::print_task_begin()
{
  std::lock_guard<std::mutex> lock(task_info_mutex);
  std::cout << "[MASTER] Beginning to execute client \'" << task_info.client_name << "\'\n";
}

void Master::print_task_finish()
{
  std::lock_guard<std::mutex> lock(task_info_mutex);
  std::cout << "[MASTER] Finished executing client \'" << task_info.client_name << "\'"
            << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - task_info.task_begin).count()
            << "ms\n";
}

void Master::print_interrupt()
{
  std::lock_guard<std::mutex> lock(task_info_mutex);
  std::cout << "[MASTER] Interrupted client \'" << task_info.client_name << "\'\n";
}

}
