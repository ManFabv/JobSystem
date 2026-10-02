//
//  main.cpp
//  JobSystem
//
//  Created by Fabricio on 2/10/26.
//

#include "JobSystem.hpp"

#include <cstdio>
#include <mutex>
#include <atomic>
#include <chrono>

// we are going to simulate a task of this amount of increments per job to
// test shared resources alternatives
const unsigned int kIncrementsPerJob = 100000;
// how many jobs we want to use
const unsigned int kJobsCount = 10;

// auxiliary struct to simulate the job data with different types of data
struct SharedCounter {
    // mutex used to lock the access to shared resources
    std::mutex counter_mutex;
    // unsafe access demonstration
    int unsafe_counter = 0;
    // mutex access demonstration
    int mutex_protected_counter = 0;
    // atomic operation access demonstration
    std::atomic<int> atomic_counter = {0};
    // local then atomic operation access demonstration
    std::atomic<int> local_then_atomic_counter = {0};
};

static void IncrementUnsafe(void* counter_data) {
    // we cast to the actual value sent
    SharedCounter* counter = static_cast<SharedCounter*>(counter_data);
    // we incremented kIncrementsPerJob times
    for (unsigned int i = 0; i < kIncrementsPerJob; ++i) {
        ++counter->unsafe_counter;
    }
}

static void IncrementWithMutex(void* counter_data) {
    // we cast to the actual value sent
    SharedCounter* counter = static_cast<SharedCounter*>(counter_data);
    // we incremented kIncrementsPerJob times
    for (unsigned int i = 0; i < kIncrementsPerJob; ++i) {
        // we first lock the value (automatically freed at the end of the loop)
        std::lock_guard<std::mutex> counter_guard_lock(counter->counter_mutex);
        // after locking it, we increment it
        ++counter->mutex_protected_counter;
    }
}

static void IncrementAtomic(void* counter_data) {
    // we cast to the actual value sent
    SharedCounter* counter = static_cast<SharedCounter*>(counter_data);
    // we incremented kIncrementsPerJob times
    for (unsigned int i = 0; i < kIncrementsPerJob; ++i) {
        // we increment the counter using <atomic> and its overloaded ++ operator
        ++counter->atomic_counter;
    }
}

static void IncrementLocalThenAtomic(void* counter_data) {
    // we cast to the actual value sent
    SharedCounter* counter = static_cast<SharedCounter*>(counter_data);
    // we use a local counter
    int local_counter = 0;
    // we incremented kIncrementsPerJob times
    for (unsigned int i = 0; i < kIncrementsPerJob; ++i) {
        // we increment the local counter
        ++local_counter;
    }
    // we increment the shared counter by the amount counted by the local counter
    counter->local_then_atomic_counter += local_counter;
}

static double RunJobsAndMeasureMS(JobSystem& job_system, JobFunction job_function, SharedCounter& shared_counter) {
    // in order to measure the milliseconds that a job takes to run, we measure the time between "now" and
    // at the end of this method after the jobs ran
    const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

    // we create kJobsCount jobs to run
    for (unsigned int i = 0; i < kJobsCount; ++i) {
        // we create the job
        const Job current_job = {job_function, &shared_counter};
        // we enqueue this new job into the system (they're not running yet)
        job_system.Submit(current_job);
    }
    // we run all the enqueued jobs and wait for all to call .join()
    job_system.RunPending();

    // we take the final time after running all the jobs
    const std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    // we now return the time in ms between start and end time
    return std::chrono::duration<double, std::milli>(end_time - start_time).count();
}

int main() {
    // expected value after all jobs increment their corresponding value
    const int expected_value = kJobsCount * kIncrementsPerJob;

    JobSystem job_system;

    SharedCounter shared_counter;

    double elapsed_ms = RunJobsAndMeasureMS(job_system, IncrementUnsafe, shared_counter);
    std::printf("IncrementUnsafe: %7d de %d (%.2f ms)\n", shared_counter.unsafe_counter, expected_value, elapsed_ms);

    elapsed_ms = RunJobsAndMeasureMS(job_system, IncrementWithMutex, shared_counter);
    std::printf("IncrementWithMutex: %7d de %d (%.2f ms)\n", shared_counter.mutex_protected_counter, expected_value, elapsed_ms);

    elapsed_ms = RunJobsAndMeasureMS(job_system, IncrementAtomic, shared_counter);
    std::printf("IncrementAtomic: %7d de %d (%.2f ms)\n", shared_counter.atomic_counter.load(), expected_value, elapsed_ms);

    elapsed_ms = RunJobsAndMeasureMS(job_system, IncrementLocalThenAtomic, shared_counter);
    std::printf("IncrementLocalThenAtomic: %7d de %d (%.2f ms)\n", shared_counter.local_then_atomic_counter.load(), expected_value, elapsed_ms);

    return 0;
}
