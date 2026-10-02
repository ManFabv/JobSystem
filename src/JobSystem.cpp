//
//  JobSystem.cpp
//  JobSystem
//
//  Created by Fabricio Manrique on 2/10/26.
//

#include "JobSystem.hpp"

#include <thread>

JobSystem::JobSystem():m_queue_head(0), m_queue_count(0) {
}


bool JobSystem::Submit(const Job& job) {
    // if we reached full capacity, we cannot enqueue any new job
    if (m_queue_count == kJobQueueCapacity) {
        return false;
    }
    // we get the tail index for the array of enqueued jobs
    const unsigned int tail_index = (m_queue_head + m_queue_count) % kJobQueueCapacity;
    // we put the job at the end of the array
    m_job_queue[tail_index] = job;
    // we increment the job count
    ++m_queue_count;
    return true;
}

void JobSystem::RunPending() {
    // we store an array of threads
    std::thread job_threads[kJobQueueCapacity];
    // index to traverse the array of threads
    unsigned int job_threads_count = 0;
    
    // we are going to store the jobs into this variable
    Job current_job;
    while(TryPopJob(current_job)) {
        // we create a thread and start the job into it
        job_threads[job_threads_count] = std::thread(current_job.function, current_job.data);
        // we increment the threads count
        ++job_threads_count;
    }
    
    // at the end, we wait for all threads to finish calling to join() function
    for(unsigned int i=0; i < job_threads_count; ++i) {
        job_threads[i].join();
    }
}

bool JobSystem::TryPopJob(Job& outJob) {
    // if we don't have any job in the array, we exit
    if (m_queue_count == 0) {
        return false;
    }
    // we get the next job from the array of queued jobs
    // (where head points is the first element)
    outJob = m_job_queue[m_queue_head];
    // we move the heading index
    m_queue_head = (m_queue_head + 1) % kJobQueueCapacity;
    // we decrement the count
    --m_queue_count;
    return true;
}


/*
 public:
     // we enqueue the job execution
     bool Submit(const Job& job);
     // we execute the jobs that are queued
     void RunPending();
     
 private:
     static constexpr unsigned int kJobQueueCapacity = 256;
     
     // we try to extract a job into outJob parameter
     // thus, needing to be a reference
     bool TryPopJob(Job& outJob);
     
     Job m_job_queue[kJobQueueCapacity];
     unsigned int m_queue_head;
     unsigned int m_queue_count;
 */
