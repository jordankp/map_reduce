#pragma once

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <chrono>

#include "error.h"
#include "dynamic_loading.h"

namespace map_reduce
{

class Master
{
  std::atomic_bool interrupted;

  struct TaskInfo
  {
    const char *client_name;
    size_t num_threads;
    std::chrono::steady_clock::time_point task_begin;
    bool is_active = false;
  };
  TaskInfo task_info;

  std::mutex task_info_mutex;

  using KeyIn = std::string;
  using KeyOut = std::string;
  using ValueIn = void *;
  using ValueOut = void *;

  using ReadFunction = std::vector<std::pair<KeyIn, ValueIn>> (*)(void);
  using WriteFunction = void (*)(const std::vector<std::pair<KeyOut, ValueOut>> &);
  using MapFunction = std::vector<std::pair<KeyOut, ValueOut>> (*)(KeyIn, ValueIn);
  using ReduceFunction = std::pair<KeyOut, ValueOut> (*)(KeyOut, const std::vector<ValueOut> &);

  struct TaskFunctions
  {
    ReadFunction read;
    WriteFunction write;
    MapFunction map;
    ReduceFunction reduce;
  };
  TaskFunctions task_funcs;

  std::vector<std::thread> workers;
  size_t workers_done_count;
  std::condition_variable event_workers_done;
  std::mutex workers_done_mutex;

  std::vector<std::pair<KeyIn, ValueIn>> map_inputs;
  size_t map_inputs_index;
  std::vector<std::vector<std::pair<KeyOut, ValueOut>>> map_outputs;

  std::unordered_map<KeyOut, std::vector<ValueOut>> reduce_inputs;
  std::unordered_map<KeyOut, std::vector<ValueOut>>::const_iterator reduce_inputs_iter;
  std::vector<std::pair<KeyOut, ValueOut>> reduce_outputs;

  std::mutex worker_inputs_mutex;
  std::mutex worker_outputs_mutex;

  enum class WorkerTask
  {
    MAP = 0,
    REDUCE,
    REST
  };
  WorkerTask current_worker_task = WorkerTask::REST;

  std::condition_variable event_worker_map;
  std::mutex event_worker_map_mutex;

  std::condition_variable event_worker_reduce;
  std::mutex event_worker_reduce_mutex;

public:

  Master() : interrupted(false) {}
  Master(const Master&) = delete;
  Master& operator=(const Master&) = delete;
  ~Master() = default;

  ErrorCode execute_task(const ClientFunctions &cl_funcs, const char *client_name, size_t num_threads);
  void interrupt();
  void print_task_info();

private:

  void init_task(const ClientFunctions &cl_funcs, const char *client_name, size_t num_threads);
  void term_task();

  void read_phase();
  void map_phase();
  void shuffle_phase();
  void reduce_phase();
  void write_phase();

  void worker_routine();
  void worker_map();
  void worker_reduce();
  void increase_done_count();

  void fake_reduce();

  void print_task_begin();
  void print_task_finish();
  void print_interrupt();
};

}
