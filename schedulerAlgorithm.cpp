#include "ResourceScheduler.h"
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
    cout << "=====================" << endl;
    for (int i = 0; i < jobSize.size(); i++)
    {
        cout << jobSize[i] << " ";
    }
    cout << endl;
    for (int i = 0; i < jobTime.size(); i++)
    {
        cout << jobTime[i] << " ";
    }
    cout << endl;

    cout << "=====================" << endl;
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
            //更新hostCoreTask
            double tmpHostCoreFinishTime = hostCoreFinishTime[0][minCoreFinishTimeNum];
            for (int j = 0; j < jobBlock[currentLongestJobPos]; j++)
            {
                hostCoreTask[0][minCoreFinishTimeNum].push_back(make_tuple(currentLongestJobPos, j, tmpHostCoreFinishTime, tmpHostCoreFinishTime + dataSize[currentLongestJobPos][j] / (Sc[currentLongestJobPos])));
                tmpHostCoreFinishTime = tmpHostCoreFinishTime + dataSize[currentLongestJobPos][j] / (Sc[currentLongestJobPos]);
            }

            jobFinishTime[currentLongestJobPos] = minCoreFinishTime + currentLongestJobTime;    //更新job完成时间
            jobCore[currentLongestJobPos] = 1;                                                //更新job使用核数
            hostCoreFinishTime[0][minCoreFinishTimeNum] = jobFinishTime[currentLongestJobPos];  //更新核的完成时间
            for (int j = 0; j < jobBlock[currentLongestJobPos]; j++)                                //更新runLoc
            {
                runLoc[currentLongestJobPos][j] = make_tuple(0, minCoreFinishTimeNum, coreBlockNum[minCoreFinishTimeNum]++);
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

       cout<<"未放入的工作数"<<remainJob.size()<<endl;
       for(int i=0;i<remainJob.size();i++)
       {
          cout<<"job号"<<remainJob[i]<<"工作时长"<<jobTime[remainJob[i]]<<endl;
      }

       for(int i=0;i<hostCore[0];i++)
       {
           cout<<"核"+to_string(i)+"的finish time"<<hostCoreFinishTime[0][i]<<endl;
       }
   /*
       for(int j=0;j<hostCore[0];j++)
       {
           cout<<"核"<<j<<"已有的块数："<<coreBlockNum[j]<<endl;
       }

   */

   //根据将剩下的job分1个核 2个核 n个核运行时其结束时间早晚来判断是否分核

    vector<vector<double>> jobCoreTime;               //jobNum->coreNum-><time> 分别计算剩下job使用不同核时，每个核所需要的总时间
    vector<vector<double>> jobCoreFinishTime;         //jobNum->coreNum-><FinishTime> =  jobCoreTime + hostCoreFinishTime[core_th]
    priority_queue<double, vector<double>, greater<double> > tmpHostCoreFinishTime;

    jobCoreFinishTime.resize(remainJob.size());
    jobCoreTime.resize(remainJob.size());
    vector<double> sortCoreFinishTime;               //记录当前分类情况
    for (int i = 0; i < remainJob.size(); i++)             //number数
 //   for(int i=remainJob.size()-1;i>=0;i--)
    {
        jobCoreTime[i].resize(hostCore[0]);  //j=0,1,2,3...分别代表1，2，3，4个核
        for (int j = 0; j < hostCore[0]&&j<jobBlock[remainJob[i]]; j++)
        {
            jobCoreTime[i][j] = jobSize[remainJob[i]] / ((Sc[remainJob[i]] * (1 - alpha * j)) * (j + 1));
                   cout<<"job总数:"<<jobSize[remainJob[i]]<<"job 速度"<<Sc[remainJob[i]]*(1-alpha*(j))<<endl;
                   cout<<"job"+to_string(remainJob[i])+"使用"+to_string(j+1)+"个核时每个核所需平均时间"<<jobCoreTime[i][j]<<endl;
        }

        //找出每个job使用不同核时的最小完成时间记录在jobCoreFinishTime[i][j]中
        jobCoreFinishTime[i].resize(hostCore[0],10000);
        for (int j = 0; j < hostCore[0] && j < jobBlock[remainJob[i]]; j++)              //核数
        {
            //将完成时间按从小到大 放入优先队列；
            for (int i = 0; i < hostCore[0]; i++)
            {
                tmpHostCoreFinishTime.push(hostCoreFinishTime[0][i]);
            }

            //记录排序后的完成时间
            sortCoreFinishTime.resize(hostCore[0], 0);
            for (int i = 0; i < hostCore[0]; i++)
            {
                sortCoreFinishTime[i] = tmpHostCoreFinishTime.top();
                tmpHostCoreFinishTime.pop();
            }

            while (!tmpHostCoreFinishTime.empty())
                tmpHostCoreFinishTime.pop();

            for (int i = 0; i < hostCore[0]; i++)
            {
                tmpHostCoreFinishTime.push(hostCoreFinishTime[0][i]);
            }


            double jthCoreFinishTime = tmpHostCoreFinishTime.top();

            for (int k = 0; k < j; k++)
            {
                tmpHostCoreFinishTime.pop();
                jthCoreFinishTime = tmpHostCoreFinishTime.top();
            }

                   cout<<"第"<<j+1<<"小的核完成时间是"<<jthCoreFinishTime<<endl;
            jobCoreFinishTime[i][j] = jobCoreTime[i][j] + jthCoreFinishTime;
                   cout<<"job"<<remainJob[i]<<"使用"<<j+1<<"个核时的最小完成时间"<<jobCoreFinishTime[i][j]<<endl;

            while (!tmpHostCoreFinishTime.empty())
                tmpHostCoreFinishTime.pop();
        }

        double jobCoreMinFinishTime = *min_element(jobCoreFinishTime[i].begin(), jobCoreFinishTime[i].end()); //找到最小的完成时间
        int jobCoreMinFinishTimePos = min_element(jobCoreFinishTime[i].begin(), jobCoreFinishTime[i].end()) - jobCoreFinishTime[i].begin() + 1; //找出最小的使用核数
        cout<<"Job"<<remainJob[i]<<"最小完成时间"<<jobCoreMinFinishTime<<"最小核数"<<jobCoreMinFinishTimePos<<"起始核完成时间"<< sortCoreFinishTime[jobCoreMinFinishTimePos-1]<<endl;

        jobCore[remainJob[i]] = jobCoreMinFinishTimePos;   //更新job最小核数

        vector<double> remainJobIBlockTime;      //根据预先确定的核数来计算block块的运行时间
        remainJobIBlockTime.resize(jobBlock[remainJob[i]]);

        //    double tmp1=0;

            //计算job块大小
        for (int j = 0; j < jobBlock[remainJob[i]]; j++)
        {
            remainJobIBlockTime[j] = dataSize[remainJob[i]][j] / (Sc[remainJob[i]] * (1 - alpha * (jobCoreMinFinishTimePos - 1)));
            //       cout<<"块大小"<<dataSize[remainJob[i]][j]<<"速度大小"<<Sc[remainJob[i]]*(1-alpha*(jobCoreMinFinishTimePos-1))<<"所需时间"<<remainJobIBlockTime[j]<<" ";

            //       tmp1=tmp1+remainJobIBlockTime[j];
        }
        //      cout<<tmp1<<endl;

          //找到最小的前jobCoreMinFinishTimePos 的核所在位置后，将block按贪心算法分配其中
          //构造优先级队列按finishiTime <finishTime,coreNumber>,其大小为jobMinFinishTimePos 大小 然后将Job块按从大到小放入其中coreNumber中

          //对所有核的完成时间建堆
        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> coreJobBlock;
        for (int k = 0; k < hostCore[0]; k++)
        {
            coreJobBlock.push(make_pair(hostCoreFinishTime[0][k], k));
        }

        //找出选中的核并将其开始时间初始化为sortCoreFinishTime[jobCoreMinFinishTimePos-1];
        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> coreCurrentJobBlock;
        for (int k = 0; k < jobCoreMinFinishTimePos; k++)
        {
            //将选中最小的核 记录其核号并将其完成时间更新为最新的。
            coreCurrentJobBlock.push(make_pair(sortCoreFinishTime[jobCoreMinFinishTimePos - 1], coreJobBlock.top().second));
            //   cout<<"选中的最小核时间"<<coreCurrentJobBlock.top().first<<"最小核号"<<coreJobBlock.top().second<<endl;
            hostCoreFinishTime[0][coreJobBlock.top().second] = coreCurrentJobBlock.top().first;
            coreJobBlock.pop();
        }

        //       for(int j=0;j<hostCore[0];j++)
        //       {
        //           cout<<"当前核结束时间"<<hostCoreFinishTime[0][j]<<"  ";
        //       }
        //       cout<<endl;

               //开始放block
        vector<int> jobBlockFlag;                 //记录block的访问情况初始化为0
        jobBlockFlag.resize(jobBlock[remainJob[i]], 0);

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
            //更新hostCoreTask
            hostCoreTask[0][tmp.second].push_back(make_tuple(remainJob[i], maxRemainJobBlockPos, hostCoreFinishTime[0][tmp.second] - maxRemainJobBlockTime, hostCoreFinishTime[0][tmp.second]));
            //更新堆
            coreCurrentJobBlock.pop();
            coreCurrentJobBlock.push(make_pair(hostCoreFinishTime[0][tmp.second], tmp.second));
            //     cout<<"更新的核时间"<<hostCoreFinishTime[0][tmp.second]<<"核号"<<tmp.second<<endl;
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
            //       cout<<maxRemainJobBlockTime<<endl;
            jobBlockFlag[maxRemainJobBlockPos] = 1;
        }

        //    for(int i=0;i<hostCore[0];i++)
         //   {
        //        cout<<"核"+to_string(i)+"的finish time"<<hostCoreFinishTime[0][i]<<endl;
         //   }

            //更新job完成时间
        double jobFinishTimeTmp = 0;

        while (!coreCurrentJobBlock.empty())
        {
            jobFinishTimeTmp = coreCurrentJobBlock.top().first;
            coreCurrentJobBlock.pop();
        }

        jobFinishTime[remainJob[i]] = jobFinishTimeTmp;

    }
    /*
        for(int i=0;i<hostCore[0];i++)
        {
            cout<<"核"+to_string(i)+"的finish time"<<hostCoreFinishTime[0][i]<<endl;
        }

    */
}
