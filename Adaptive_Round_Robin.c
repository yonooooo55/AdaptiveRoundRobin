#include <stdio.h>

#define MAX_PROCESS 10000

//Self explanatory, this here is a structure that represents a process along with its attributes.
typedef struct {
    int process_id;     
    int burst_time;     
    int remaining_time; 
    int arrival_time;
    int waiting_time;
    int turnaround_time;
    int response_time;  
    int processed;
    int finish_time;
    int fin_Flag;
} Process;

/*METHOD SIGNATURES for cleaner code*/
void round_robin(Process processes[], int num_processes, int* context_switch_count);
void sortArray(Process processes[], int n);
int STSCalc(Process processes[], int n); 
int UncompletedProcessCount(Process processes[], int num_process);
void PrintResults(Process processes[], int num_process, int context_switch_count);


//Main Programme 
int main() {
    
    int num_processes;              // Store the number of processes 
    Process processes[MAX_PROCESS]; // Array to store processes 
    int context_switch_count = 0;   // Variable to count context switches

    int cst = 0;
   
    //HEADER OF PROGRAM
    printf("CPU Scheduling Method: Adaptive Round Robin by SAROJ HIRANWAL, Dr. K.C. ROY\n");

    //INPUT: No. of processes
    printf("\nEnter the no of processes: ");
    scanf("%d", &num_processes);

    //BURST TIME : For each process entered 
    for (int i = 0 ; i < num_processes; i ++) 
    {
        //Set up params 
        processes[i].process_id = i + 1;                        //Pid 
        printf("\nEnter burst time for process %d: ", i+1);     //Enter the burst time for the process
        scanf("%d", &processes[i].burst_time);                  //Store it in the process structure
        printf("Enter arrival time for process %d: ", i+1);     //Enter the arrival time for process 
        scanf("%d", &processes[i].arrival_time);                //Arrival time
        processes[i].response_time = 0;                         //Default Response time is 0
        processes[i].processed = 0;                             //FLAG to track if process has been executed
        processes[i].fin_Flag = 0;                              //FLAG to track if process Response time has been recorded
        processes[i].finish_time = 0;                           //Finish time set to 0
        processes[i].remaining_time = processes[i].burst_time;  //Remaining time will be initially set to burst time to be deducted from
    }

    
    

    //CALLING the ROUND ROBIN ALGO function and passing in the processes list, number of processes and the address of the context_switch_count to keep track of context switches
    round_robin(processes, num_processes, &context_switch_count);
    
    
    //PRINT STATEMENTS FOR ALL PROCESSES
    printf("\nList of Processes:\n");
    printf("%-8s%-14s%-12s%-13s%-16s%-13s%-14s\n", "PROCESS", "ARRIVAL TIME", "BURST TIME", "FINISH TIME", "TURNAROUND TIME", "WAITING TIME", "RESPONSE TIME");
    for (int i = 0; i < num_processes; ++i) {
        printf("%-8d%-14d%-12d%-13d%-16d%-13d%-14d\n", 
            processes[i].process_id,
            processes[i].arrival_time, 
            processes[i].burst_time,
            processes[i].finish_time,
            processes[i].turnaround_time,
            processes[i].waiting_time,
            processes[i].response_time);
    }

    //PRINT AGGREGATED TIME CALCULATIONS
    PrintResults(processes, num_processes, context_switch_count);

}


//ADAPTIVE ROUND ROBI HAPPENS HERE
void round_robin(Process processes[], int num_processes, int* context_switch_count) {
        
        int curr_time = 0;  //SETS CURRENT TIME to ZERO

        while (1) {
            
            //CHECKS IF ALL PROCESSES Finshed flag is MARKED WITH "-1" indicating process finished execution
            int All_Processes_Finished_Flag = UncompletedProcessCount(processes, num_processes);                //CHECKS IF ALL PROCESSES ARE FINISHED?
            if(All_Processes_Finished_Flag == 0){
                break;                                                                                          //BREAKS THE WHILE LOOP IF ALL PROCESSES ARE FINISHED
            }
            
            //AN Array the stores the list of processes that arrived
            Process arrival_queue[num_processes];   
            
            //RETRIEVING THE PROCESSES FROM THE MASTER LIST and adding it into the arrival queue this is to simulate an arrival queue for processes that just arrived
            int arrival_count = 0;  
            for(int i = 0; i < num_processes; i++){
                if (processes[i].arrival_time <= curr_time && processes[i].fin_Flag != -1){
                    arrival_queue[arrival_count] = processes[i];
                    arrival_count++;
                }
            }


            //IF ARRIVAL QUEUE IS NOT EMPTY
            if (arrival_count > 0){

                sortArray(arrival_queue, arrival_count);                //SORT all processes in ascending order based on burst time
                int STS = STSCalc(arrival_queue, arrival_count);        //SMART TIME SLICE calculation function

                for (int i = 0 ; i < arrival_count ; i++){              //LOOPS through all arrived process
                                        
                    //UPDATE RESPONSE TIME
                    int index = arrival_queue[i].process_id - 1;        //RETRIEVING THE PROCESSES FROM THE MASTER LIST
                    if (processes[index].processed != -1){
                        processes[index].response_time = curr_time - processes[index].arrival_time;
                        processes[index].processed = -1;                
                    }

                    //PROCESS CAN FINISH IN THIS TIMESLICE
                    if (arrival_queue[i].remaining_time <= STS) {   
                        curr_time += arrival_queue[i].remaining_time;                                                   //Update current time
                        processes[index].fin_Flag = -1;                                                                 //Process finished FLAG
                        processes[index].turnaround_time = curr_time - processes[index].arrival_time;                   //Turnaround time calculation 
                        processes[index].waiting_time = processes[index].turnaround_time - processes[index].burst_time; //Waiting time calculation 
                        processes[index].finish_time = curr_time;
                    }

                    //PROCESS CANNOT FINISH IN THIS TIMESLICE
                    else {
                        int remainder_time = arrival_queue[i].remaining_time - STS; //CALCULATION OF THE REMAINDER TIME
                        processes[index].remaining_time = remainder_time;   //UPDATE REMINAING BURST TIME
                        curr_time += STS;                                   //UPDATE TIME SLICE 
                    }
               
                    //CONTEXT SWITCH TRACKED
                    (*context_switch_count) ++;
                }
            }

            else 
            {
                curr_time ++; //time increment
            }    
        }
           
}


//SORTS AN ARRAY IN ASCENDING BY BURST TIME
void sortArray(Process processes[], int n) {
    int i, j;      //counters
    Process temp; //struct to temp store a process

    
    for (i = 0; i < n - 1; i++) { 
        for (j = 0; j < n - i - 1; j++) { 
            if (processes[j].remaining_time > processes[j + 1].remaining_time) {
                // Swap processes[j] and processes[j+1]
                temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

//SMART TIME SLICE CALCULATOR
int STSCalc(Process processes[], int n) {
    
    int STS = 0; //Variable to store the calculated smart time slice
    if (n % 2 == 0)  //IF NUMBER PROCESS IS EVEN: FIND AVERAGE TIME: Add all the processes burst time in the arrival queue 
    {
        for (int i = 0; i < n; i++) {
            STS += processes[i].remaining_time; //Add each process burst time 
        }
        STS = (STS / n) + (STS % n);        //Because in C, any decimal values are rounded down, so i add the modulo value to round up as per requirements for decimal time slices.
    }
    //IF NUMBER OF PROCESS IS ODD: FIND MIDDLE BURST TIME
    else 
    {
        int index = n/2;
        STS = processes[index].remaining_time;  //Based on middle index get the middle process burst time
    }
    return STS;
}

//PRINTS THE METRICS WHICH IS THE AVERAGE
void PrintResults (Process processes[], int num_process, int context_switch_count) {
    
    float total_wait_time = 0;
    float total_turn_around_time = 0;
    float total_response_time = 0;
    

    for (int i = 0 ; i < num_process; i++)
    {
        total_wait_time += processes[i].waiting_time;
        total_turn_around_time += processes[i].turnaround_time;
        total_response_time += processes[i].response_time;
    }

    printf("\nThe Average Waiting time: %.2f \n", total_wait_time / num_process);
    printf("The Average Turnaround time: %.2f \n", total_turn_around_time / num_process);
    printf("The Average Response time: %.2f \n", total_response_time / num_process);
    printf("Context Switch Count#: %d \n", context_switch_count - 1);
}

//COUNT OF UNCOMPLETED PROCESSES
//Check the list of processes and returns the count of how many are not finished
int UncompletedProcessCount(Process processes[], int num_process){
    int count = 0;
    for(int i = 0; i < num_process; i++){
        if (processes[i].fin_Flag != -1){
            count ++;
        }
    }
    return count;
}

