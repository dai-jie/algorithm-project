#include "ResourceScheduler.h"

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

void ResourceScheduler::transferToHost0() {
	int allCoreNum = 0;
	for (int i = 0; i < numHost; i++) {
		allCoreNum += hostCore[i];
	}
	core2hostcore.resize(allCoreNum);
	for (int i = 0, index = 0; i < numHost; i++) {
		for (int j = 0; j < hostCore[i]; j++) {
			core2hostcore[index].first = i;
			core2hostcore[index].second = j;
		}
	}
	hostCore[0] = allCoreNum;

	//修改几个存储结果的变量的维数
	hostCoreTask.resize(1);
	hostCoreTask[0].resize(hostCore[0]);

	hostCoreFinishTime.resize(1);
	hostCoreFinishTime[0].resize(hostCore[0]);
}

// give datasize, runloc,hostCoreTask rewrite these variables with core2hostTable core2coreTable
void ResourceScheduler::resolveFromHost0(ResourceScheduler& databackup)
{
	// task2's runLoc
	vector<vector<tuple<int, int, int>>> runlocTemp;
	vector<vector<double>> hostCoreFinishTimeTemp;
	vector<vector<vector<tuple<int, int, double, double>>>> hostCoreTaskTemp;

	runlocTemp.resize(databackup.numJob);
	for (size_t i = 0; i < databackup.numJob; i++)
	{
		runlocTemp[i].resize(databackup.jobBlock[i]);
	}
	for (int i = 0; i < numJob; i++)
	{
		for (int j = 0; j < jobBlock[i]; j++)
		{
			auto& runloci = runLoc[i][j];
			int coreid = std::get<1>(runloci);
			int rankid = std::get<2>(runloci);
			int hostid = core2hostcore[coreid].first;
			int realcoreid = core2hostcore[coreid].second;
			runlocTemp[i][j] = std::make_tuple(hostid, realcoreid, rankid);
		}
	}
	//调整blockTime
	// 
	//
	// 
	// 计算hostCoreTask;

	//计算jobFinishTime
	// 
	//计算hostcoreFinishTimeTemp
	hostCoreFinishTimeTemp.resize(numHost);
	for (int i = 0; i < numHost; i++)
	{
		hostCoreFinishTimeTemp[i].resize(databackup.hostCore[i]);
	}
	for (int i = 0; i < hostCore[0]; i++)
	{
		int coreid = i;
		int hostid = core2hostcore[coreid].first;
		int realcoreid = core2hostcore[coreid].second;
		hostCoreFinishTimeTemp[hostid][realcoreid] = hostCoreFinishTime[0][i];
	}









}

void ResourceScheduler::adjustTime()
{
	//存储每个job的开始block
	vector<vector<int> > jobStartBlock;
	jobStartBlock.resize(numJob);

	for (int i = 0; i < numHost; i++)
	{
		for (int corenum = 0; corenum < hostCore[i]; corenum++)
		{
			vector<int> firstFlag;
			firstFlag.resize(numJob, 0);
			for (int taskIdx = 0; taskIdx < hostCoreTask[i][corenum].size(); taskIdx++)
			{
				auto& hostCoreTaski = hostCoreTask[i][corenum][taskIdx];
				int job = std::get<0>(hostCoreTaski);
				int block = std::get<1>(hostCoreTaski);
				if (firstFlag[job] == 0)
				{
					jobStartBlock[job].push_back(block);
					firstFlag[job] = 1;
				}
			}
		}
	}

	//给出job block，求出host core rank
	//给出host core ，得出begin time
	for (int jobi = 0; jobi < numJob; jobi++)
	{
		double max_begin = 0;
		for (int i = 0; i < jobStartBlock.size(); i++)
		{
			int block = jobStartBlock[jobi][i];
			auto& runLocE = runLoc[jobi][block];
			int hostid = std::get<0>(runLocE);
			int coreid = std::get<1>(runLocE);
			int rank = std::get<2>(runLocE);
			rank--;
			auto& hostCoreTaskE = hostCoreTask[hostid][coreid][rank];
			double begin_time = std::get<2>(runLocE);
			max_begin = max(max_begin, begin_time);
		}

		for (auto& block : jobStartBlock[jobi])
		{
			auto& runLocE = runLoc[jobi][block];
			int hostid = std::get<0>(runLocE);
			int coreid = std::get<1>(runLocE);
			int rank = std::get<2>(runLocE);
			rank--;
			auto& hostCoreTaskE = hostCoreTask[hostid][coreid][rank];
			double begin_time = std::get<2>(runLocE);

			if (begin_time < max_begin)
			{
				for (int i = rank; i < hostCoreTask[hostid][coreid].size(); i++)
				{
					auto& hostCoreTaskE = hostCoreTask[hostid][coreid][i];
					double& new_time = std::get<2>(hostCoreTaskE);
					new_time += max_begin - begin_time;
				}
			}
		}
	}

	for (auto& i : hostCoreTask)
	{
		for (auto& j : i)
		{
			for (auto& k : j)
			{
				double& end_time = std::get<3>(k);
				int jobid = std::get<0>(k);
				int blockid = std::get<1>(k);
				end_time += blockPlusTrans[jobid][blockid];
			}
		}
	}

	//修改jobFinishTime
	for (int jobi = 0; jobi < numJob; jobi++)
	{
		double max_finish = 0;
		for (int blocki = 0; blocki < jobBlock[jobi]; blocki++)
		{
			auto& runLocE = runLoc[jobi][blocki];
			int hostid = std::get<0>(runLocE);
			int coreid = std::get<1>(runLocE);
			int rank = std::get<2>(runLocE);
			rank--;
			auto& hostCoreTaskE = hostCoreTask[hostid][coreid][rank];
			double finish_time = std::get<3>(hostCoreTaskE);
			max_finish = max(max_finish, finish_time);
		}
		jobFinishTime[jobi] = max_finish;
	}

	//修改hostcoreFinishTime
	for (int i = 0; i < numHost; i++)
	{
		for (int core = 0; core < hostCore[i]; core++)
		{
			auto& lastTask = hostCoreTask[i][core].back();
			hostCoreFinishTime[i][core] = std::get<3>(lastTask);
		}
	}
}

void ResourceScheduler::calculateBlockPTrans()
{
	//vector<vector<double> > blockPlusTrans; 未初始化。
}