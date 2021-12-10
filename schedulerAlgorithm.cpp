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
			//���� finishLine 
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
			//���� finishLine 
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
    cout << "ƽ�����ʱ��" << averageJobTime << endl;

    vector<int> coreBlockNum;
    vector<int> jobFlag;
    vector<int> remainJob;                            //��¼δʹ�õ�job��

    jobFlag.resize(numJob, 0);                         //��¼ÿ��job�ķ��ʼ�¼
    coreBlockNum.resize(hostCore[0], 0);               //��¼ÿ�����������е�block��

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
    jobFlag[currentLongestJobPos] = 1;                                 //��ǰ�job�ѷ���

    for (int i = numJob; i > 0; i--)
    {
        if (minCoreFinishTime + currentLongestJobTime < averageJobTime * 1.01)  //����job���ú�
        {
            //����hostCoreTask
            double tmpHostCoreFinishTime = hostCoreFinishTime[0][minCoreFinishTimeNum];
            for (int j = 0; j < jobBlock[currentLongestJobPos]; j++)
            {
                hostCoreTask[0][minCoreFinishTimeNum].push_back(make_tuple(currentLongestJobPos, j, tmpHostCoreFinishTime, tmpHostCoreFinishTime + dataSize[currentLongestJobPos][j] / (Sc[currentLongestJobPos])));
                tmpHostCoreFinishTime = tmpHostCoreFinishTime + dataSize[currentLongestJobPos][j] / (Sc[currentLongestJobPos]);
            }

            jobFinishTime[currentLongestJobPos] = minCoreFinishTime + currentLongestJobTime;    //����job���ʱ��
            jobCore[currentLongestJobPos] = 1;                                                //����jobʹ�ú���
            hostCoreFinishTime[0][minCoreFinishTimeNum] = jobFinishTime[currentLongestJobPos];  //���º˵����ʱ��
            for (int j = 0; j < jobBlock[currentLongestJobPos]; j++)                                //����runLoc
            {
                runLoc[currentLongestJobPos][j] = make_tuple(0, minCoreFinishTimeNum, coreBlockNum[minCoreFinishTimeNum]++);
            }
            minCoreFinishTime = *min_element(hostCoreFinishTime[0].begin(), hostCoreFinishTime[0].end());     //���º˵ĵ�ǰ״̬
            minCoreFinishTimeNum = min_element(hostCoreFinishTime[0].begin(), hostCoreFinishTime[0].end()) - hostCoreFinishTime[0].begin();
        }
        else
        {
            remainJob.push_back(currentLongestJobPos);
        }

        currentLongestJobTime = 0;                         //���µ�ǰ�job
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

       cout<<"δ����Ĺ�����"<<remainJob.size()<<endl;
       for(int i=0;i<remainJob.size();i++)
       {
          cout<<"job��"<<remainJob[i]<<"����ʱ��"<<jobTime[remainJob[i]]<<endl;
      }

       for(int i=0;i<hostCore[0];i++)
       {
           cout<<"��"+to_string(i)+"��finish time"<<hostCoreFinishTime[0][i]<<endl;
       }
   /*
       for(int j=0;j<hostCore[0];j++)
       {
           cout<<"��"<<j<<"���еĿ�����"<<coreBlockNum[j]<<endl;
       }

   */

   //���ݽ�ʣ�µ�job��1���� 2���� n��������ʱ�����ʱ���������ж��Ƿ�ֺ�

    vector<vector<double>> jobCoreTime;               //jobNum->coreNum-><time> �ֱ����ʣ��jobʹ�ò�ͬ��ʱ��ÿ��������Ҫ����ʱ��
    vector<vector<double>> jobCoreFinishTime;         //jobNum->coreNum-><FinishTime> =  jobCoreTime + hostCoreFinishTime[core_th]
    priority_queue<double, vector<double>, greater<double> > tmpHostCoreFinishTime;

    jobCoreFinishTime.resize(remainJob.size());
    jobCoreTime.resize(remainJob.size());
    vector<double> sortCoreFinishTime;               //��¼��ǰ�������
    for (int i = 0; i < remainJob.size(); i++)             //number��
 //   for(int i=remainJob.size()-1;i>=0;i--)
    {
        jobCoreTime[i].resize(hostCore[0]);  //j=0,1,2,3...�ֱ����1��2��3��4����
        for (int j = 0; j < hostCore[0]&&j<jobBlock[remainJob[i]]; j++)
        {
            jobCoreTime[i][j] = jobSize[remainJob[i]] / ((Sc[remainJob[i]] * (1 - alpha * j)) * (j + 1));
                   cout<<"job����:"<<jobSize[remainJob[i]]<<"job �ٶ�"<<Sc[remainJob[i]]*(1-alpha*(j))<<endl;
                   cout<<"job"+to_string(remainJob[i])+"ʹ��"+to_string(j+1)+"����ʱÿ��������ƽ��ʱ��"<<jobCoreTime[i][j]<<endl;
        }

        //�ҳ�ÿ��jobʹ�ò�ͬ��ʱ����С���ʱ���¼��jobCoreFinishTime[i][j]��
        jobCoreFinishTime[i].resize(hostCore[0],10000);
        for (int j = 0; j < hostCore[0] && j < jobBlock[remainJob[i]]; j++)              //����
        {
            //�����ʱ�䰴��С���� �������ȶ��У�
            for (int i = 0; i < hostCore[0]; i++)
            {
                tmpHostCoreFinishTime.push(hostCoreFinishTime[0][i]);
            }

            //��¼���������ʱ��
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

                   cout<<"��"<<j+1<<"С�ĺ����ʱ����"<<jthCoreFinishTime<<endl;
            jobCoreFinishTime[i][j] = jobCoreTime[i][j] + jthCoreFinishTime;
                   cout<<"job"<<remainJob[i]<<"ʹ��"<<j+1<<"����ʱ����С���ʱ��"<<jobCoreFinishTime[i][j]<<endl;

            while (!tmpHostCoreFinishTime.empty())
                tmpHostCoreFinishTime.pop();
        }

        double jobCoreMinFinishTime = *min_element(jobCoreFinishTime[i].begin(), jobCoreFinishTime[i].end()); //�ҵ���С�����ʱ��
        int jobCoreMinFinishTimePos = min_element(jobCoreFinishTime[i].begin(), jobCoreFinishTime[i].end()) - jobCoreFinishTime[i].begin() + 1; //�ҳ���С��ʹ�ú���
        cout<<"Job"<<remainJob[i]<<"��С���ʱ��"<<jobCoreMinFinishTime<<"��С����"<<jobCoreMinFinishTimePos<<"��ʼ�����ʱ��"<< sortCoreFinishTime[jobCoreMinFinishTimePos-1]<<endl;

        jobCore[remainJob[i]] = jobCoreMinFinishTimePos;   //����job��С����

        vector<double> remainJobIBlockTime;      //����Ԥ��ȷ���ĺ���������block�������ʱ��
        remainJobIBlockTime.resize(jobBlock[remainJob[i]]);

        //    double tmp1=0;

            //����job���С
        for (int j = 0; j < jobBlock[remainJob[i]]; j++)
        {
            remainJobIBlockTime[j] = dataSize[remainJob[i]][j] / (Sc[remainJob[i]] * (1 - alpha * (jobCoreMinFinishTimePos - 1)));
            //       cout<<"���С"<<dataSize[remainJob[i]][j]<<"�ٶȴ�С"<<Sc[remainJob[i]]*(1-alpha*(jobCoreMinFinishTimePos-1))<<"����ʱ��"<<remainJobIBlockTime[j]<<" ";

            //       tmp1=tmp1+remainJobIBlockTime[j];
        }
        //      cout<<tmp1<<endl;

          //�ҵ���С��ǰjobCoreMinFinishTimePos �ĺ�����λ�ú󣬽�block��̰���㷨��������
          //�������ȼ����а�finishiTime <finishTime,coreNumber>,���СΪjobMinFinishTimePos ��С Ȼ��Job�鰴�Ӵ�С��������coreNumber��

          //�����к˵����ʱ�佨��
        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> coreJobBlock;
        for (int k = 0; k < hostCore[0]; k++)
        {
            coreJobBlock.push(make_pair(hostCoreFinishTime[0][k], k));
        }

        //�ҳ�ѡ�еĺ˲����俪ʼʱ���ʼ��ΪsortCoreFinishTime[jobCoreMinFinishTimePos-1];
        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> coreCurrentJobBlock;
        for (int k = 0; k < jobCoreMinFinishTimePos; k++)
        {
            //��ѡ����С�ĺ� ��¼��˺Ų��������ʱ�����Ϊ���µġ�
            coreCurrentJobBlock.push(make_pair(sortCoreFinishTime[jobCoreMinFinishTimePos - 1], coreJobBlock.top().second));
            //   cout<<"ѡ�е���С��ʱ��"<<coreCurrentJobBlock.top().first<<"��С�˺�"<<coreJobBlock.top().second<<endl;
            hostCoreFinishTime[0][coreJobBlock.top().second] = coreCurrentJobBlock.top().first;
            coreJobBlock.pop();
        }

        //       for(int j=0;j<hostCore[0];j++)
        //       {
        //           cout<<"��ǰ�˽���ʱ��"<<hostCoreFinishTime[0][j]<<"  ";
        //       }
        //       cout<<endl;

               //��ʼ��block
        vector<int> jobBlockFlag;                 //��¼block�ķ��������ʼ��Ϊ0
        jobBlockFlag.resize(jobBlock[remainJob[i]], 0);

        //�ҵ����blcok���ŵ���С�ĺ�����
        double maxRemainJobBlockTime = *max_element(remainJobIBlockTime.begin(), remainJobIBlockTime.end());
        int maxRemainJobBlockPos = max_element(remainJobIBlockTime.begin(), remainJobIBlockTime.end()) - remainJobIBlockTime.begin();
        jobBlockFlag[maxRemainJobBlockPos] = 1;

        for (int k = 0; k < jobBlock[remainJob[i]]; k++)
        {
            pair<double, int> tmp = coreCurrentJobBlock.top();

            hostCoreFinishTime[0][tmp.second] = hostCoreFinishTime[0][tmp.second] + maxRemainJobBlockTime;
            //����runLoc
            runLoc[remainJob[i]][maxRemainJobBlockPos] = make_tuple(0, tmp.second, coreBlockNum[tmp.second]++);
            //����hostCoreTask
            hostCoreTask[0][tmp.second].push_back(make_tuple(remainJob[i], maxRemainJobBlockPos, hostCoreFinishTime[0][tmp.second] - maxRemainJobBlockTime, hostCoreFinishTime[0][tmp.second]));
            //���¶�
            coreCurrentJobBlock.pop();
            coreCurrentJobBlock.push(make_pair(hostCoreFinishTime[0][tmp.second], tmp.second));
            //     cout<<"���µĺ�ʱ��"<<hostCoreFinishTime[0][tmp.second]<<"�˺�"<<tmp.second<<endl;
                 //�������block
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
        //        cout<<"��"+to_string(i)+"��finish time"<<hostCoreFinishTime[0][i]<<endl;
         //   }

            //����job���ʱ��
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
            cout<<"��"+to_string(i)+"��finish time"<<hostCoreFinishTime[0][i]<<endl;
        }

    */
}
