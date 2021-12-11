#include "ResourceScheduler.h"
#define RandomGenerate 0
#define ReadFromFile 1

using namespace std;


ResourceScheduler::ResourceScheduler(int tasktype, int caseID, int generatetype) {
	if (generatetype == RandomGenerate) {
		taskType = tasktype;
		srand((int)time(0));
		numJob = 20, numHost = 4, alpha = 0.07;
		if (taskType == 2) St = 500;
		int minCore = 3, maxCore = 20; //number of cores
		//int minCore = 3, maxCore = 20;
		int minBlock = 5, maxBlock = 20; //number of blocks
		int minSize = 50, maxSize = 200; //size of blocks
		double minSpeed = 20, maxSpeed = 80;
		hostCore.resize(numHost);
		jobBlock.resize(numJob);
		Sc.resize(numJob);
		dataSize.resize(numJob);
		location.resize(numJob);


		cout << "\n\n-----------Generator starts.--------------\n\n";

		cout << "numJob = " << numJob << ", numHost = " << numHost << ", alpha = " << alpha;
		if (taskType == 2) cout << ", St = " << St;
		cout << "\n\n";
    
		cout << "hostCore:\n";
		for (int i = 0; i < numHost; i++) {
			do {
				hostCore[i] = rand() % (maxCore - minCore + 1) + minCore;
				//cout << hostCore[i] << endl;
			} while (alpha >= 1.0 / (hostCore[i] - 1));

			cout << hostCore[i] << " ";
		}

		cout << "\n\njobBlockNumber:\n";
		for (int i = 0; i < numJob; i++) {
			jobBlock[i] = rand() % (maxBlock - minBlock + 1) + minBlock;
			cout << jobBlock[i] << " ";
		}


		cout << "\n\njobCalculatingSpeed:\n";
		for (int i = 0; i < numJob; i++)
		{
			Sc[i] = rand() % int(maxSpeed - minSpeed + 1) + minSpeed;
			cout << Sc[i] << " ";
		}

		cout << "\n\nblockDataSize:\n";
		for (int i = 0; i < numJob; i++)
		{
			dataSize[i].resize(jobBlock[i]);
			for (int j = 0; j < jobBlock[i]; j++) {
				dataSize[i][j] = rand() % (maxSize - minSize + 1) + minSize;
				//dataSize[i][j] = fabs(std::normal_distribution<double> n(4, 1.5));
				std::cout << dataSize[i][j] << " ";
			}
		}

    

		std::cout << "\njobBlockInitialLocation:\n";
		for (int i = 0; i < numJob; i++)
		{
			location[i].resize(jobBlock[i]);
			for (int j = 0; j < jobBlock[i]; j++) {
				location[i][j] = rand() % numHost;
				cout << location[i][j] << " ";
			}
			cout << endl;
		}

		cout << "\n\n-----------Generator ends.--------------\n\n";
	}
	else if (generatetype == ReadFromFile) {
		taskType = tasktype;
		string filePath = ".\\input\\task" + to_string(taskType) + "_case" + to_string(caseID) + ".txt";
		//	string buffer = getcwd(NULL, 0);
		//	cout << buffer;

		if (freopen(filePath.c_str(), "r", stdin) == NULL)
			cout << filePath;
		cin >> numJob >> numHost >> alpha;
		if (taskType == 2)
			cin >> St;
		hostCore.resize(numHost);
		for (int i = 0; i < numHost; i++)
			cin >> hostCore[i];

		jobBlock.resize(numJob);
		for (int i = 0; i < numJob; i++)
			cin >> jobBlock[i];

		Sc.resize(numJob);
		for (int i = 0; i < numJob; i++)
			cin >> Sc[i];

		dataSize.resize(numJob);
		for (int i = 0; i < numJob; i++) {
			dataSize[i].resize(jobBlock[i]);
			for (int j = 0; j < jobBlock[i]; j++)
				cin >> dataSize[i][j];
		}
		location.resize(numJob);                    //任务一无此项
		for (int i = 0; i < numJob; i++) {
			location[i].resize(jobBlock[i]);
			for (int j = 0; j < jobBlock[i]; j++)
				cin >> location[i][j];
		}
	}

	jobFinishTime.resize(numJob, 0);
	jobCore.resize(numJob);

	runLoc.resize(numJob);
	for (int i = 0; i < numJob; i++)
		runLoc[i].resize(jobBlock[i]);

	hostCoreTask.resize(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreTask[i].resize(hostCore[i]);

	hostCoreFinishTime.resize(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreFinishTime[i].resize(hostCore[i], 0);

}

void ResourceScheduler::schedule() {
	vector<vector<int>> hostCoreBlock(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreBlock[i].resize(hostCore[i], 0);

	for (int i = 0; i < numJob; i++) {
		set<pair<int, int>> allocatedJobCore;
		for (int j = 0; j < jobBlock[i]; j++) {
			int hid = rand() % numHost;
			int cid = rand() % hostCore[hid];
			allocatedJobCore.insert({ hid,cid });
			runLoc[i][j] = make_tuple(hid, cid, hostCoreBlock[hid][cid]++);
		}
		/*for (int j = 0; j < jobBlock[i]; j++)
			finishTime[i]+=*/
		jobFinishTime[i] = rand() % 200;
		jobCore[i] = allocatedJobCore.size();
	}


	for (int i = 0; i < numHost; i++) {
		for (int j = 0; j < hostCore[i]; j++) {
			int numTask = rand() % 10 + 1;
			hostCoreTask[i][j].resize(numTask);
			for (int k = 0; k < numTask; k++) {
				int jid = rand() % numJob;
				int bid = rand() % jobBlock[jid];
				int endTime = hostCoreFinishTime[i][j] + rand() % 100 + 1;
				hostCoreTask[i][j][k] = make_tuple(jid, bid, hostCoreFinishTime[i][j], endTime);
				hostCoreFinishTime[i][j] = endTime;
			}
		}
	}
}

void ResourceScheduler::outputSolutionFromBlock() {
	cout << "\nTask2 Solution (Block Perspective) of Team08:\n\n";
	for (int i = 0; i < numJob; i++) {
		double speed = g(jobCore[i]);
		cout << "Job" << i << " obtains " << jobCore[i] << " cores (speed=" << speed << ") and finishes at time " << jobFinishTime[i] << ": \n";
		for (int j = 0; j < jobBlock[i]; j++) {
			cout << "\tBlock" << j << ": H" << get<0>(runLoc[i][j]) << ", C" << get<1>(runLoc[i][j]) << ", R" << get<2>(runLoc[i][j]) << " (time=" << fixed << setprecision(2) << dataSize[i][j] / speed << ")" << " \n";
		}
		cout << "\n";
	}

	cout << "The maximum finish time: " << *max_element(jobFinishTime.begin(), jobFinishTime.end()) << "\n";
	cout << "The total response time: " << accumulate(jobFinishTime.begin(), jobFinishTime.end(), 0.0) << "\n\n";
}

void ResourceScheduler::outputSolutionFromCore() {
	cout << "\nTask2 Solution (Core Perspective) of Team08:\n\n";
	double maxHostTime = 0, totalRunningTime = 0.0;
	for (int i = 0; i < numHost; i++) {
		double hostTime = *max_element(hostCoreFinishTime[i].begin(), hostCoreFinishTime[i].end());
		maxHostTime = max(hostTime, maxHostTime);
		totalRunningTime += accumulate(hostCoreFinishTime[i].begin(), hostCoreFinishTime[i].end(), 0.0);
		cout << "Host" << i << " finishes at time " << hostTime << ":\n\n";
		for (int j = 0; j < hostCore[i]; j++) {
			cout << "\tCore" << j << " has " << hostCoreTask[i][j].size() << " tasks and finishes at time " << hostCoreFinishTime[i][j] << ":\n";
			for (int k = 0; k < hostCoreTask[i][j].size(); k++) {
				cout << "\t\tJ" << setw(2) << setfill('0') << get<0>(hostCoreTask[i][j][k]) << ", B" << setw(2) << setfill('0') << get<1>(hostCoreTask[i][j][k]) << ", runTime " << fixed << setprecision(1) << setw(5) << setfill('0') << get<2>(hostCoreTask[i][j][k]) << " to " << fixed << setprecision(1) << setw(5) << setfill('0') << get<3>(hostCoreTask[i][j][k]) << "\n";
			}
			cout << "\n";
		}
		cout << "\n\n";
	}
	cout << "The maximum finish time of hosts: " << maxHostTime << "\n";
	cout << "The total efficacious running time: " << totalRunningTime << "\n";
	cout << "Utilization rate: " << totalRunningTime / accumulate(hostCore.begin(), hostCore.end(), 0.0) / maxHostTime << "\n\n";
}

void ResourceScheduler::visualization() {

}

double ResourceScheduler::g(int e) {
	return 1 - alpha * (e - 1);
}
