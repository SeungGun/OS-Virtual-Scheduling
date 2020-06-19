#pragma (disable:4996)
#include <stdio.h>
#include <stdlib.h> 
#define MAX_TICK 50
typedef struct PCB {
	int pid;
	int begin_tick;
	int burst_tick;
	int finish;
	int turnAround;
	int waiting;
	int response;
}PCB;
PCB *process;
int size;
int ticks[MAX_TICK] = { 0 };

void read_proc_list(const char* file_name) {
	FILE* fp;
	if ((fp = fopen(file_name, "r")) == NULL) {
		printf("Fail to open file\n");
		return;
	}
	fscanf(fp, "%d", &size);
	printf("size : %d\n", size);
	process = (PCB*)malloc(sizeof(PCB)*size);
	for (int i = 0; i < size; i++) {
		fscanf(fp, "%d %d %d", &process[i].pid, &process[i].begin_tick, &process[i].burst_tick);	
		printf("pid : %d, begin : %d, burst : %d\n", process[i].pid, process[i].begin_tick, process[i].burst_tick);
	}
	fclose(fp);
}
void set_schedule(int method) {
	if (method == 1) { // FCFS (non-preemptive)
		int cnt = 0;
		int arrive_min = process[0].begin_tick;
		int burst_min = 0;
		int index = -1;

		for (int i = 1; i < size; i++) {
			if (arrive_min > process[i].begin_tick) {
				burst_min = process[i].burst_tick;
				arrive_min = process[i].begin_tick;
				index = i;
			}
		}
		if (arrive_min == process[0].begin_tick&&arrive_min>0) index = 0;
		if (arrive_min > 0) {
			for (int i = 0; i < arrive_min; i++) {
				ticks[cnt] = -1;
				cnt++;
			}
			process[index].waiting = arrive_min;
			process[index].response = arrive_min;
		}
		for (int i = 0; i < size; i++) {
			int tmp = process[i].burst_tick;
			while (tmp--) {
				ticks[cnt] = i + 1;
				cnt++;				
			}	
			process[i].finish = cnt;
			process[i].turnAround = process[i].finish - process[i].begin_tick;
			if (index == i)continue;
			process[i].waiting = 0;
			process[i].response = 0;
			for (int j = 0; j < i; j++) {
				process[i].waiting += process[j].burst_tick ;
				process[i].response = process[i].waiting;

			}
		}	
	}
	/*---------------------------------------------------------------------------------------------*/
	else if (method == 2) {// SJF (non-preemptive)
		int arrive_min = process[0].begin_tick;
		int burst_min = process[0].burst_tick;
		int cnt = 0;
		int index = 0;
		int *temp = (int*)malloc(sizeof(int)*size);
		int *idxs = (int*)malloc(sizeof(int)*size);
		int *checkUse = (int*)malloc(sizeof(int)*size);
		int Notzero = -1;

		for (int i = 0; i < size; i++) idxs[i] = -1;
		for (int i = 0; i < size; i++) checkUse[i] = 0;

		for (int i = 1; i < size; i++) {
			if (arrive_min > process[i].begin_tick) {
				burst_min = process[i].burst_tick;
				arrive_min = process[i].begin_tick;
				index = i;
			}
		}
		for (int i = 0; i < process[index].begin_tick; i++) {
			ticks[cnt] = -1;
			cnt++;
		}
		printf("arrive_min : %d\n", arrive_min);
		if (process[index].begin_tick > 0) {
			//process[index].waiting = arrive_min;
			//process[index].response = arrive_min;
			Notzero = index;
		}
		int firstExe_burst = burst_min;
		while (burst_min--) {
			ticks[cnt] = index + 1;
			cnt++;
		}
		process[index].finish = cnt;
		process[index].turnAround = process[index].finish - process[index].begin_tick;
		process[index].waiting = 0;
		process[index].response = process[index].finish - process[index].begin_tick;

		for (int i = 0; i < size; i++) {
			temp[i] = process[i].burst_tick;
		}
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size - 1; j++) {
				if (temp[j] > temp[j + 1]) {
					int t = temp[j];
					temp[j] = temp[j + 1];
					temp[j + 1] = t;
				}
			}
		}
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
					if (temp[i] == process[j].burst_tick && checkUse[j] == 0) {
							idxs[i] = j;
							checkUse[j] = 1;
							break;						
					}
			}
		}
		printf("%d\n", index);
		for (int i = 0; i < size; i++) {
			if (firstExe_burst == temp[i] && index == idxs[i]) {
				printf("i: %d\n", i);
				continue;
			}
			else {
				burst_min = temp[i];
				while (burst_min--) {
					ticks[cnt] = idxs[i] + 1;
					cnt++;
				}
				process[idxs[i]].finish = cnt;
				process[idxs[i]].turnAround = process[idxs[i]].finish - process[idxs[i]].begin_tick;
				//if (Notzero == i) continue;
				printf("not zero : %d ?, i = %d\n", Notzero,i);
				process[idxs[i]].waiting += arrive_min;
				

				for (int j = 0; j < size; j++) {
					process[j].waiting = process[j].finish - process[j].begin_tick - process[j].burst_tick;
					process[j].waiting += arrive_min;
				
					process[j].response = process[j].waiting;
				}
			}
		}
		
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	else if (method == 3) { // SRTF (preemptive)
		int cnt = 0;
		int arrive_min = process[0].begin_tick;
		int burst_min = process[0].burst_tick;
		int index = 0;
		int total_burst = 0;
		int *arriveSort = (int*)malloc(sizeof(int)*size);
		int *remainTimes = (int*)malloc(sizeof(int*)*size);
		int *startTimes = (int*)malloc(sizeof(int*)*size);

		int *idxs = (int*)malloc(sizeof(int)*size);
		int *checkUse = (int*)malloc(sizeof(int)*size);

		for (int i = 0; i < size; i++) idxs[i] = -1;
		for (int i = 0; i < size; i++) checkUse[i] = 0;
		for (int i = 0; i < size; i++) startTimes[i] = -1;

		for (int i = 0; i < size; i++) arriveSort[i] = process[i].begin_tick;
		for (int i = 0; i < size; i++) remainTimes[i] = process[i].burst_tick;

		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size - 1; j++) {
				if (arriveSort[j] > arriveSort[j + 1]) {
					int tmp = arriveSort[j];
					arriveSort[j] = arriveSort[j + 1];
					arriveSort[j + 1] = tmp;
				}
			}
			total_burst += process[i].burst_tick;
		}
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				if (arriveSort[i] == process[j].begin_tick && checkUse[j] == 0) {
					idxs[i] = j;
					checkUse[j] = 1;
					break;
				}
			}
		}
		if (arriveSort[0] > 0) { //if begin time of MIN of arrive time is not 0
			for (int k = 0; k < arriveSort[0]; k++) {
				ticks[cnt] = -1;
				cnt++;
			}
			total_burst += arriveSort[0];
			
		}
		
		int i = 0;
		int convert = 0; // determines whether while-loop finish or not  
		int execute = process[i].burst_tick;

		while (execute-- && cnt != total_burst) { 
			if (startTimes[i] == -1) startTimes[i]= cnt;
			ticks[cnt] = i + 1;
			cnt++;
			remainTimes[i] = execute;
			if (remainTimes[i] == 0) {
				process[i].finish = cnt;
				process[i].turnAround = process[i].finish - process[i].begin_tick;
				process[i].waiting = process[i].finish - process[i].begin_tick - process[i].burst_tick;
				process[i].response = startTimes[i] - process[i].begin_tick;
			}

			for (int indx = 0; indx < size; indx++) {
				if (remainTimes[indx] != 0) {
					convert = 1;
					break;
				}
			}
			if (convert == 0) break;

			if (cnt == arriveSort[i+1]) {
				if (remainTimes[i] > remainTimes[i+1]) {
					execute = remainTimes[i+1];
					i++;
					continue;
				}
				else {
					execute = remainTimes[i];
					if (execute == 0) {
						int min_remain = 50;
						for (int j = 0; j < size; j++) {
							if (min_remain > remainTimes[j] && remainTimes[j] != 0) {
								min_remain = remainTimes[j];
								i = j;
							}
						}
						execute = min_remain;
					}
					continue;
				}
			}
			else {
				if (execute == 0) {
					int min_remain = 50;
					for (int j = 0; j < size; j++) {
						if (min_remain > remainTimes[j]&&remainTimes[j] != 0) {
							min_remain = remainTimes[j];
							i = j;
						}
					}
					execute = min_remain;
					continue;
				}
			}
		}
	}

}
int do_schedule(int tick) { 
	for (int i = 0; i < MAX_TICK; i++) {
		 if (ticks[tick] == -1) return 0;
		 else if (ticks[tick] == 0) return -1;

		if (tick == i) {
			return ticks[i];
		}
	}
	return -1;
}
void print_performance() {
	printf("=======================================================================================\n");
	printf("PID \t begin \t finish \t Turn around time \t Waiting time \t Response time\n");
	printf("=======================================================================================\n");
	for (int i = 0; i < size; i++)
		printf("%d \t  %d  \t    %d \t\t\t %d \t\t   %d \t\t %d\n", i + 1, process[i].begin_tick, process[i].finish, process[i].turnAround, process[i].waiting, process[i].response);
	double avgTAT = 0;
	double avgWT = 0;
	double avgRT = 0;
	for (int i = 0; i < size; i++) {
		avgTAT += process[i].turnAround;
		avgWT += process[i].waiting;
		avgRT += process[i].response;
	}
	printf("----------------------------------------------------------------------------------------\n");
	printf("average: \t\t\t\t %.2lf \t\t %.2lf \t\t %.2lf\n", avgTAT / size, avgWT / size, avgRT / size);
	printf("========================================================================================\n");
}
int tick = 0;
int main() {
	read_proc_list("proc_list.txt");
	set_schedule(1);
	while (2) {
		int res = do_schedule(tick);
		if (res < 0 || tick > 50) break;
		printf("[tick:%04d] CPU is allocated to process No.%02d\n", tick, res);
		tick++;
	}
	print_performance(); 

}
