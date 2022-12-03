#pragma once
#include "params.h"
#include "MurmurHash.h"
#include<bits/stdc++.h>
using namespace std;
class BIT_CM_ver3 :public Sketch{
    public:
    class Layer{
        char *counter;
        int len,bit,mx,md,d,hashseed,level;
        //[0,md]--didn't overflow，[md+1,mx]--overflow
        Layer*upper;
	    uint64_t (*hash)(const void*, int32_t, uint32_t);
        int ask(int pos){
            char*p=counter;int k;
            pos*=bit;
            k=pos>>3;
            p+=k;pos=pos&7;
            k=0;
            pos=7-pos;
            for(int i=0;i<bit;i++){
                k<<=1;
                k+=(p[0]>>pos)&1;
                pos--;
                if(pos<0)p++,pos=7;
            }
            return k;
        }
        void cover(int pos,int c){
            if(c>mx||c<0||pos>len){
                fprintf(stderr,"%d cover error %d %d",level,pos,c);
                exit(-1);
            }
            char*p=counter;int k;
            pos*=bit;
            k=pos>>3;
            p+=k;pos=pos&7;
            pos=7-pos;
            for(int i=bit-1;i>=0;i--){
                k=((c>>i)&1)<<pos;
                p[0]=(p[0]&cov0[pos])^k;
                pos--;
                if(pos<0)p++,pos+=8;
            }
        }
    public:
        Layer(int _w=0,int _bit=0,int _d=3,int _hashseed=1000,Layer*up=NULL,int _id=0){
            len=_w/(_bit+1);
            bit=_bit+1;
            d=_d;
            hashseed=_hashseed;
            counter=new char[len*bit/8+8]();
            upper=up;
            level=_id;
            hash=MurmurHash32;
            mx=(1<<(bit))-1;
            md=mx*mxtimes/4;//---------------------mxtimes - threadsold
        }
        int Len(){return len;}
        void Insert(int pos,int k){
            // printf("%d insert %d %d\n",level,pos,k);
            int nw=ask(pos);
            if(nw+k>mx){
                // puts("insert up");
                for(int index,i=0;i<d;i++){
                    index=((unsigned)MurmurHash32(&pos,4, hashseed+i))%upper->Len();
                    upper->Insert(index,1);
                }
                k-=mx-md;
            }
            cover(pos,nw+k);
        }
        int *fin;
        void Fin(int*&thislayer,int&thislen){
            thislen=len;
            fin=new int[len]();
            bool*bit=new bool[len]();
            thislayer=fin;
            for(int k=0;k<len;k++){
                thislayer[k]=ask(k);
                bit[k]=thislayer[k]>md;
            }
            int*uplayer,uplen;
            if(upper!=NULL)upper->Fin(uplayer,uplen);
            else return;
            int before=0,last=0;
            // printf("we get %d:%d\n",upper->id,uplen);
            // for(int k=0;k<uplen;k++)printf("%d ",uplayer[k]);puts("");
            int *id=new int[uplen]();
            int *have=new int[uplen]();
            int i,k;
            for(k=0;k<len;k++){
                if(bit[k]){
                    before++;
                    for(int index,i=0;i<d;i++){
                        index=((unsigned)MurmurHash32(&k,4, hashseed+i))%uplen;
                        id[index]^=k;
                        have[index]++;
                    }
                }
            }
            queue<int>q;
            while(!q.empty())q.pop();
            for(k=0;k<uplen;k++)if(have[k]==1)q.push(k);
            while(!q.empty()){
                k=q.front();q.pop();
                if(have[k]!=1)continue;
                int val=uplayer[k];
                k=id[k];
                fin[k]+=val*(mx-md);
                bit[k]=0;
                for(int index,i=0;i<d;i++){
                    index=((unsigned)MurmurHash32(&k,4, hashseed+i))%uplen;
                    id[index]^=k;
                    have[index]--;
                    uplayer[index]-=val;
                    if(have[index]==1)q.push(index);
                }
            }
            for(k=0;k<len;k++){
                if(bit[k]){
                    last++;
                }
            }
            //printf("fin %d:%d/%d/%d, ",level,last,before,len);
            std::cout<<"fin "<<level<<":";
            std::cout<<"\033[34m"<<last<<"\033[0m"<<"/"<<before<<"/"<<len<<", ";
        }
        void check(){
            for(int k=0;k<len;k++)printf("%d ",ask(k));
            puts("");
            // for(int k=0;k<len*bit/8+8;k++)printf("%d ",counter[k]);
            // puts("");
        }
    }layer[8];
    int w,d,hashseed;bool unzip=0;
    int*counter;int len;
    int *c0;
public:
    BIT_CM_ver3(int _w,int _d,int _hashseed=1000){
        _w*=8;
        w=_w;
        d=_d;
        hashseed=_hashseed;
        const int sz[]={0,(int)(mem[1]*1.23),(int)(mem[2]*1.23),(int)(mem[3]*1.23),(int)(mem[4]*1.23),(int)(mem[5]*1.23)};
        layer[0]=Layer(w-sz[1]*4-sz[2]*4-sz[3]*3-sz[4]*3,4,3,hashseed,&layer[1],0);
        layer[1]=Layer(sz[1]*4,3,3,hashseed*2,&layer[2],1);
        layer[2]=Layer(sz[2]*4,3,3,hashseed*3,&layer[3],2);
        layer[3]=Layer(sz[3]*3,2,3,hashseed*4,&layer[4],3);
        layer[4]=Layer(sz[4]*3,2,3,hashseed*5,&layer[5],4);
        layer[5]=Layer(sz[5]*10,9,3,hashseed*6,NULL,5);
        c0=new int[layer[0].Len()]();
    }
	void Insert(const char* str){
		int g=0,index[MAX_HASH_NUM];
		for (int i = 0; i < d; i++){
			index[i] = ((unsigned)MurmurHash32(str, KEY_LEN, hashseed + i)) % layer[0].Len();
            layer[0].Insert(index[i],1);
            c0[index[i]]++;
		}
	}
	int Query(const char* str){
        if(unzip==0){
            layer[0].Fin(counter,len);
            unzip=1;
            int sum=0;
            for(int i=0;i<len;i++)
            if(counter[i]!=c0[i])sum++;
            //printf("\nerr:%d/%d\n",sum,len);
            std::cout<<"\n";
            std::cout<<"\033[34merr:"<<sum<<"/"<<"\033[0m"<<len<<std::endl;
        }
		int temp,res=INT_MAX;
		int index[MAX_HASH_NUM];
		for (int  i=0;i<d;i++){
			index[i] = ((unsigned)MurmurHash32(str, KEY_LEN, hashseed + i)) % len;
			temp = counter[index[i]];
			res=min(res,temp);
		}
		return res;
	}
    void check(){
        layer[0].check();
        layer[1].check();
        // layer[2].check();
        // layer[3].check();
        // layer[4].check();
    }
};