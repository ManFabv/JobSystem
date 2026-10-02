//
//  JobSystem.hpp
//  JobSystem
//
//  Created by Fabricio Manrique on 2/10/26.
//

#ifndef JOBSYSTEM_HPP
#define JOBSYSTEM_HPP

// defining an Alias for the JobData
typedef void* JobData;
// defining an Alias for the JobFunction which receives a JobData
typedef void (*JobFunction)(JobData);

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
    static constexpr unsigned int kJobQueueCapacity = 256;
    
    // we try to extract a job into outJob parameter
    // thus, needing to be a reference
    bool TryPopJob(Job& outJob);
    
    Job m_job_queue[kJobQueueCapacity];
    unsigned int m_queue_head;
    unsigned int m_queue_count;
};

#endif // JOBSYSTEM_HPP
