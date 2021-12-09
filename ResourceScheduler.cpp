#include "ResourceScheduler.h"
#include <random>
#define RandomGenerate 0
#define ReadFromFile 1

using namespace std;


ResourceScheduler::ResourceScheduler(int tasktype, int caseID, int generatetype) {
	if (generatetype == RandomGenerate) {
		taskType = tasktype;
		srand((int)time(0));
		numJob = 10, numHost = 1, alpha = 0.01;
		if (taskType == 2) St = 500;
		int minCore = 3, maxCore = 10; //number of cores
		//int minCore = 3, maxCore = 20;
		int minBlock = 20, maxBlock = 50; //number of blocks
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

	cout << "\n\n-----------Generator ends.--------------\n\n";
}

void ResourceScheduler::Initial() {

	blockTime.resize(numJob);
	for (int i = 0; i < numJob; i++) {
		blockTime[i].resize(hostCore[0]);
		for (int j = 0; j < hostCore[0]; j++) {
			blockTime[i][j].resize(jobBlock[i]);
			for (int k = 0; k < jobBlock[i]; k++) {
				blockTime[i][j][k] = dataSize[i][k] / (ResourceScheduler::g(j + 1) * Sc[i]);
			}
		}

	}

	/*for (int i = 0; i < numJob; i++) {
		;
		for (int j = 0; j < hostCore[0]; j++) {

			for (int k = 0; k < jobBlock[i]; k++) {
				cout << blockTime[i][j][k] << " ";
			}
			cout << endl;
		}

	}*/

	blockSch.resize(numJob);
	for (int i = 0; i < numJob; i++)
	{
		blockSch[i].resize(hostCore[0]);
		for (int j = 0; j < hostCore[0]; j++)
		{
			blockSch[i][j].resize(j + 1, 0);
		}
	}

	for (int i = 0; i < numJob; i++)
	{
		for (int j = 0; j < hostCore[0]; j++)
		{
			int coreNum = j + 1;
			priority_queue<double, vector<double>, greater<double> > timeQueue;
			for (int k = 0; k < coreNum; k++) timeQueue.push(0.0);
			sort(blockTime[i][j].begin(), blockTime[i][j].end(), greater<double>());
			for (int k = 0; k < jobBlock[i]; k++)
			{
				double finishTime = timeQueue.top() + blockTime[i][j][k];
				timeQueue.pop();
				timeQueue.push(finishTime);
			}
			for (int k = 0; k < coreNum; k++)
			{
				blockSch[i][j][k] = timeQueue.top();
				timeQueue.pop();
			}

		}
	}

	/*for (int i = 0; i < numJob; i++)
	{
		for (int j = 0; j < hostCore[0]; j++)
		{
			for (int k = 0; k < blockSch[i][j].size(); k++)
			{
				cout << blockSch[i][j][k] << " ";

			}
			cout << endl;

		}
	}*/
	jobSize.resize(numJob, 0);                  //2021++
	jobTime.resize(numJob, 0);

	for (int i = 0; i < numJob; i++)
	{
		dataSize[i].resize(jobBlock[i]);
		for (int j = 0; j < jobBlock[i]; j++) {
			jobSize[i] = jobSize[i] + dataSize[i][j];     //每个job的总大小 2021++
		}
	}

	for (int i = 0; i < numJob; i++)
	{
		jobTime[i] = jobSize[i] / Sc[i];     //每个job所需要的时间
	}
	




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
	cout << "\nTask2 Solution (Block Perspective) of Teaching Assistant:\n\n";
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
	cout << "\nTask2 Solution (Core Perspective) of Teaching Assistant:\n\n";
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



void ResourceScheduler::scheduleTwoStep() {
	priority_queue<pair<double, int>, vector<pair<double, int>>, less<pair<double, int> > > heapJob; //max-heap
	priority_queue<double, vector<double>, greater<double> > heapFinish; //min-heap
	for (int i = 0; i < hostCore[0]; i++) heapFinish.push(0.0);

	double finishLine = 0;
	for (int i = 0; i < numJob; i++) {
		finishLine += blockSch[i][0][0];
		heapJob.push(make_pair(blockSch[i][0][0], i));
	}

	finishLine /= double(hostCore[0]);
	double perfectTime = finishLine;

	int coreNum = 0;
	while (coreNum < hostCore[0] && (!heapJob.empty()))
	{
		priority_queue<pair<double, int>, vector<pair<double, int>>, less<pair<double, int> > > heapJobTemp;
		while (!heapJob.empty())
		{
			pair<double, int> jobi = heapJob.top();
			heapJob.pop();
			vector<double> blockFinish;
			vector<double> jobFinish;
			for (int i = 0; i <= coreNum; i++) blockFinish.push_back(blockSch[jobi.second][coreNum][i]);
			double maxFinish = 0;
			for (int i = 0; i <= coreNum; i++)
			{
				maxFinish = max(maxFinish, heapFinish.top());
				jobFinish.push_back(heapFinish.top());
				heapFinish.pop();
			}
			if (maxFinish + blockFinish[coreNum] <= finishLine)
			{
				for (int i = 0; i <= coreNum; i++)
				{
					jobFinish[i] = maxFinish + blockFinish[i];
					heapFinish.push(jobFinish[i]);
				}
			}
			else
			{
				for (int i = 0; i <= coreNum; i++) heapFinish.push(jobFinish[i]);
				heapJobTemp.push(jobi);
			}
		}
		heapJob = heapJobTemp;



		if (coreNum == hostCore[0] - 1 && (!heapJob.empty()))
		{
			//扩容 finishLine 
			coreNum = 0;
			double augLine = 0;
			int heapJobTempSize = heapJobTemp.size();
			while (!heapJobTemp.empty())
			{
				augLine += heapJobTemp.top().first;
				heapJobTemp.pop();
			}
			augLine /= double(heapJobTempSize);
			finishLine += augLine;
			//finishLine = augLine + heapFinish.top();
		}
		else
		{
			coreNum++;
		}
	}

	cout << "All cores finish time: " << endl;
	double infactTime = 0;
	while (!heapFinish.empty())
	{
		if (heapFinish.top() > infactTime) {
			infactTime = heapFinish.top();
		}
		cout << heapFinish.top() << ' ';
		heapFinish.pop();
	}
	cout << endl;

	cout << "infactTime / perfectTime = " << infactTime / perfectTime << endl;
}

void ResourceScheduler::scheduleTwoStep2() {
	priority_queue<pair<double, int>, vector<pair<double, int>>, less<pair<double, int> > > heapJob; //max-heap
	priority_queue<double, vector<double>, greater<double> > heapFinish; //min-heap
	for (int i = 0; i < hostCore[0]; i++) heapFinish.push(0.0);

	double finishLine = 0;
	for (int i = 0; i < numJob; i++) {
		finishLine += blockSch[i][0][0];
		heapJob.push(make_pair(blockSch[i][0][0], i));
	}

	finishLine /= double(hostCore[0]);
	double perfectTime = finishLine;

	int coreNum = 0;
	while ((!heapJob.empty()))
	{
		priority_queue<pair<double, int>, vector<pair<double, int>>, less<pair<double, int> > > heapJobTemp;
		while (!heapJob.empty())
		{
			pair<double, int> jobi = heapJob.top();
			heapJob.pop();
			vector<double> blockFinish;
			vector<double> jobFinish;
			for (int i = 0; i <= coreNum; i++) blockFinish.push_back(blockSch[jobi.second][coreNum][i]);
			double maxFinish = 0;
			for (int i = 0; i <= coreNum; i++)
			{
				maxFinish = max(maxFinish, heapFinish.top());
				jobFinish.push_back(heapFinish.top());
				heapFinish.pop();
			}
			if (maxFinish + blockFinish[coreNum] <= finishLine)
			{
				for (int i = 0; i <= coreNum; i++)
				{
					jobFinish[i] = maxFinish + blockFinish[i];
					heapFinish.push(jobFinish[i]);
				}
			}
			else
			{
				for (int i = 0; i <= coreNum; i++) heapFinish.push(jobFinish[i]);
				heapJobTemp.push(jobi);
			}
		}
		heapJob = heapJobTemp;



		if ((!heapJob.empty()))
		{
			//扩容 finishLine 
			double augLine = 0;
			int heapJobTempSize = heapJobTemp.size();
			while (!heapJobTemp.empty())
			{
				augLine += heapJobTemp.top().first;
				heapJobTemp.pop();
			}
			augLine /= double(heapJobTempSize);
			finishLine += augLine;
			//finishLine = augLine + heapFinish.top();
		}
		else
		{
			;
		}
	}

	//cout << "All cores finish time: " << endl;
	double infactTime = 0;
	while (!heapFinish.empty())
	{
		if (heapFinish.top() > infactTime) {
			infactTime = heapFinish.top();
		}
		cout << heapFinish.top() << ' ';
		heapFinish.pop();
	}
	cout << endl;
	cout << "infactTime / perfectTime = " << infactTime / perfectTime << endl;
}

void ResourceScheduler::scheduleDeng() {
	double averageJobTime;
	double totalJobTime = 0;
	for (int i = 0; i < numJob; i++)
	{
		totalJobTime = totalJobTime + jobTime[i];
	}
	averageJobTime = totalJobTime / hostCore[0];
	cout << "平均完成时间" << averageJobTime << endl;

	vector<int> coreBlockNum;
	vector<int> jobFlag;
	vector<int> remainJob;                            //记录未使用的job号

	jobFlag.resize(numJob, 0);                         //记录每个job的访问记录
	coreBlockNum.resize(hostCore[0], 0);               //记录每个核里面已有的block数

	double minCoreFinishTime = *min_element(hostCoreFinishTime[0].begin(), hostCoreFinishTime[0].end());
	int minCoreFinishTimeNum = min_element(hostCoreFinishTime[0].begin(), hostCoreFinishTime[0].end()) - hostCoreFinishTime[0].begin();

	double currentLongestJobTime = 0;
	int currentLongestJobPos = 0;
	for (int i = 0; i < numJob; i++)
	{
		if ((jobFlag[i] == 0) && (jobTime[i] > currentLongestJobTime))
		{
			currentLongestJobTime = jobTime[i];
			currentLongestJobPos = i;
		}
	}
	jobFlag[currentLongestJobPos] = 1;                                 //当前最长job已访问

	for (int i = numJob; i > 0; i--)
	{
		if (minCoreFinishTime + currentLongestJobTime < averageJobTime * 1.01)  //分配job给该核
		{
			jobFinishTime[currentLongestJobPos] = minCoreFinishTime + currentLongestJobTime;    //更新job完成时间
			jobCore[currentLongestJobPos] = 1;                                                //更新job使用核数
			hostCoreFinishTime[0][minCoreFinishTimeNum] = jobFinishTime[currentLongestJobPos];  //更新核的完成时间
			for (int j = 0; j < jobBlock[currentLongestJobPos]; j++)                                //更新runLoc
			{
				runLoc[currentLongestJobPos][j] = make_tuple(0, minCoreFinishTimeNum, coreBlockNum[minCoreFinishTimeNum]++);
				// hostCoreTask[0][i][j]       ??
			}
			minCoreFinishTime = *min_element(hostCoreFinishTime[0].begin(), hostCoreFinishTime[0].end());     //更新核的当前状态
			minCoreFinishTimeNum = min_element(hostCoreFinishTime[0].begin(), hostCoreFinishTime[0].end()) - hostCoreFinishTime[0].begin();
		}
		else
		{
			remainJob.push_back(currentLongestJobPos);
		}

		currentLongestJobTime = 0;                         //更新当前最长job
		for (int i = 0; i < numJob; i++)
		{
			if ((jobFlag[i] == 0) && (jobTime[i] > currentLongestJobTime))
			{
				currentLongestJobTime = jobTime[i];
				currentLongestJobPos = i;
			}
		}
		jobFlag[currentLongestJobPos] = 1;
	}

	cout << "未放入的工作数" << remainJob.size() << endl;

	for (int i = 0; i < remainJob.size(); i++)
	{
		cout << "job号" << remainJob[i] << "工作时长" << jobTime[remainJob[i]] << endl;
	}

	for (int i = 0; i < hostCore[0]; i++)
	{
		cout << "核" + to_string(i) + "的finish time" << hostCoreFinishTime[0][i] << endl;
	}
/****************************************************************************************************************************/

	//根据将剩下的job分1个核 2个核 n个核运行时其结束时间早晚来判断是否分核

	vector<vector<double>> jobCoreTime;               //jobNum->coreNum-><time> 分别计算剩下job使用不同核时，每个核所需要的总时间
	vector<vector<double>> jobCoreFinishTime;         //jobNum->coreNum-><FinishTime> =  jobCoreTime + hostCoreFinishTime[core_th]
	priority_queue<double, vector<double>, greater<double> > tmpHostCoreFinishTime;

	jobCoreFinishTime.resize(remainJob.size());
	jobCoreTime.resize(remainJob.size());
	for (int i = 0; i < remainJob.size(); i++)             //number数
	{
		jobCoreTime[i].resize(hostCore[0]);  //j=0,1,2,3...分别代表1，2，3，4个核
		for (int j = 0; j < hostCore[0]; j++)
		{
			jobCoreTime[i][j] = jobSize[remainJob[i]] / ((Sc[remainJob[i]] * (1 - alpha * j)) * (j + 1));
		//	 cout<<"job总数:"<<jobSize[remainJob[i]]<<"job 速度"<<Sc[remainJob[i]]*(1-alpha*(j))<<endl;
		//	 cout<<"job"+to_string(remainJob[i])+"使用"+to_string(j+1)+"个核时每个核所需平均时间"<<jobCoreTime[i][j]<<endl;
		}

		//找出每个job使用不同核时的最小完成时间记录在jobCoreFinishTime[i][j]中
		jobCoreFinishTime[i].resize(hostCore[0]);
		for (int j = 0; j < hostCore[0]; j++)              //核数
		{
			for (int i = 0; i < hostCore[0]; i++)
			{
				tmpHostCoreFinishTime.push(hostCoreFinishTime[0][i]);
			}

			double jthCoreFinishTime = tmpHostCoreFinishTime.top();         //用来记录倒数第j小的hostCoreFinishTime；

			for (int k = 0; k < j; k++)
			{
				tmpHostCoreFinishTime.pop();
				jthCoreFinishTime = tmpHostCoreFinishTime.top();
			}

			cout << "第" << j + 1 << "小的核完成时间是" << jthCoreFinishTime << endl;
			jobCoreFinishTime[i][j] = jobCoreTime[i][j] + jthCoreFinishTime;
			cout << "job" << remainJob[i] << "使用" << j + 1 << "个核时的最小完成时间" << jobCoreFinishTime[i][j] << endl;

			while (!tmpHostCoreFinishTime.empty())
				tmpHostCoreFinishTime.pop();
		}

		double jobCoreMinFinishTime = *min_element(jobCoreFinishTime[i].begin(), jobCoreFinishTime[i].end()); //找到最小的完成时间
		int jobCoreMinFinishTimePos = min_element(jobCoreFinishTime[i].begin(), jobCoreFinishTime[i].end()) - jobCoreFinishTime[i].begin() + 1; //找出最小的使用核数
		cout << "Job" << remainJob[i] << "最小完成时间" << jobCoreMinFinishTime << "最小核数" << jobCoreMinFinishTimePos << endl;

		vector<double> remainJobIBlockTime;      //根据预先确定的核数来计算block块的运行时间
		//double tmp1 = 0;
		remainJobIBlockTime.resize(jobBlock[remainJob[i]]);
		for (int j = 0; j < jobBlock[remainJob[i]]; j++)
		{
			remainJobIBlockTime[j] = dataSize[remainJob[i]][j] / (Sc[remainJob[i]] * (1 - alpha * (jobCoreMinFinishTimePos - 1)));
		//	cout << remainJobIBlockTime[j] << "  ";
		//	tmp1 = tmp1 + remainJobIBlockTime[j];
		}
		//cout << tmp1 << endl;
		//找到最小的前jobCoreMinFinishTimePos 的核所在位置后，将block按贪心算法分配其中
		//构造优先级队列按finishiTime <finishTime,coreNumber>,其大小为jobMinFinishTimePos 大小 然后将Job块按从大到小放入其中coreNumber中

		//对所有核的完成时间建堆
		priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> coreJobBlock;
		for (int k = 0; k < hostCore[0]; k++)
		{
			coreJobBlock.push(make_pair(hostCoreFinishTime[0][k], k));
		}

		//找出选中的核
		priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> coreCurrentJobBlock;
		for (int k = 0; k < jobCoreMinFinishTimePos; k++)
		{
			coreCurrentJobBlock.push(coreJobBlock.top());
			cout << "选中的最小核时间" << coreJobBlock.top().first << "最小核号" << coreJobBlock.top().second << endl;
			coreJobBlock.pop();
		}

		//开始放block
		vector<int> jobBlockFlag;                 //记录block的访问情况初始化为0
		jobBlockFlag.resize(jobBlock[remainJob[i]]);
		for (int k = 0; k < jobBlock[remainJob[i]]; k++)
			jobBlockFlag[k] = 0;

		//找到最长的blcok安排到最小的核上面
		double maxRemainJobBlockTime = *max_element(remainJobIBlockTime.begin(), remainJobIBlockTime.end());
		int maxRemainJobBlockPos = max_element(remainJobIBlockTime.begin(), remainJobIBlockTime.end()) - remainJobIBlockTime.begin();
		jobBlockFlag[maxRemainJobBlockPos] = 1;

		for (int k = 0; k < jobBlock[remainJob[i]]; k++)
		{
			pair<double, int> tmp = coreCurrentJobBlock.top();

			hostCoreFinishTime[0][tmp.second] = hostCoreFinishTime[0][tmp.second] + maxRemainJobBlockTime;
			//更新runLoc
			runLoc[remainJob[i]][maxRemainJobBlockPos] = make_tuple(0, tmp.second, coreBlockNum[tmp.second]++);

			//更新堆
			coreCurrentJobBlock.pop();
			coreCurrentJobBlock.push(make_pair(hostCoreFinishTime[0][tmp.second], tmp.second));
			//更新最大block
			maxRemainJobBlockTime = 0;
			for (int k = 0; k < jobBlock[remainJob[i]]; k++)
			{
				if ((jobBlockFlag[k] == 0) && (remainJobIBlockTime[k] > maxRemainJobBlockTime))
				{
					maxRemainJobBlockTime = remainJobIBlockTime[k];
					maxRemainJobBlockPos = k;
				}
			}
			jobBlockFlag[maxRemainJobBlockPos] = 1;
		}
	}

	for (int i = 0; i < hostCore[0]; i++)
	{
		cout << "核" + to_string(i) + "的finish time" << hostCoreFinishTime[0][i] << endl;
	}
	double maxFinshTime = 0;
	for (size_t i = 0; i < hostCore[0]; i++)
	{
		maxFinshTime = max(maxFinshTime, hostCoreFinishTime[0][i]);
	}
	cout << maxFinshTime/averageJobTime << endl;
}