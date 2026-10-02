//
//  JobSystem.hpp
//  JobSystem
//
//  Created by Fabricio on 2/10/26.
//

#ifndef JOBSYSTEM_HPP
#define JOBSYSTEM_HPP

// defining an Alias for the JobFunction which receives a pointer to its data
typedef void (*JobFunction)(void* job_data);

struct Job {
    JobFunction function;
    void* data;
};

class JobSystem {
public:
    JobSystem();
    // we enqueue the job execution
    bool Submit(const Job& job);
    // we execute the jobs that are queued
    void RunPending();

private:
    // power of 2 on purpose: the modulo becomes a bitwise AND
    static constexpr unsigned int kJobQueueCapacity = 256;

    // we try to extract a job into out_job parameter
    // thus, needing to be a reference
    bool TryPopJob(Job& out_job);

    // fixed size ring buffer, no heap allocations
    Job m_job_queue[kJobQueueCapacity];
    // index of the next job to pop
    unsigned int m_queue_head;
    // amount of jobs in the queue
    unsigned int m_queue_count;
};

#endif // JOBSYSTEM_HPP
