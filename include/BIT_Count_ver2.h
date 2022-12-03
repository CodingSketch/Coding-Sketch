#pragma once
#include "params.h"
#include "MurmurHash.h"
#include<bits/stdc++.h>
using namespace std;
class BIT_C :public Sketch{
    public:
    class Layer{
        char *counter;
        char *bitmap;
        int len,bit,mx,mn,d,hashseed,level;
        Layer*upper;
	    uint64_t (*hash)(const void*, int32_t, uint32_t);
        int askbit(int pos){
            char*p=bitmap;int k;
            k=pos>>3;
            p+=k;pos=pos&7;
            pos=7-pos;
            return (p[0]>>pos)&1;
        }
        void coverbit(int pos,int c){
            if(c>1||c<0){
                fprintf(stderr,"%d coverbit error %d %d",level,pos,c);
                exit(-1);
            }
            char*p=bitmap;int k;
            k=pos>>3;
            p+=k;pos=pos&7;
            pos=7-pos;
            k=c<<pos;
            p[0]=(p[0]&cov0[pos])^k;
        }
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
            if(k>mx)k-=1<<bit;
            return k;
        }
        void cover(int pos,int c){
            if(c>mx||c<mn){
                fprintf(stderr,"%d cover error %d %d",level,pos,c);
                exit(-1);
            }
            // printf("cover %d %d\n",pos,c);
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
            bit=_bit;
            d=_d;
            hashseed=_hashseed;
            counter=new char[len*_bit/8+8]();
            bitmap=new char[len/8+8]();
            upper=up;
            level=_id;
            hash=MurmurHash32;
            mx=(1<<(bit-1))-1;
            mn=-mx;
        }
        int Len(){return len;}
        void Insert(int pos,int k){
            // printf("%d insert %d %d\n",id,pos,k);
            int nw=ask(pos);
            if(nw+k>mx){
                for(int index,i=0;i<d;i++){
                    index=((unsigned)MurmurHash32(&pos,4, hashseed+i))%upper->Len();
                    upper->Insert(index,1);
                }
                k-=mx+1;
                coverbit(pos,1);
            }
            if(nw+k<mn){
                for(int index,i=0;i<d;i++){
                    index=((unsigned)MurmurHash32(&pos,4, hashseed+i))%upper->Len();
                    upper->Insert(index,-1);
                }
                k+=mx+1;
                coverbit(pos,1);
            }
            cover(pos,nw+k);
        }
        int *fin;
        void Fin(int*&thislayer,int&thislen){
            thislen=len;
            fin=new int[len]();
            thislayer=fin;
            for(int k=0;k<len;k++)
                thislayer[k]=ask(k);
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
                if(askbit(k)){
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
                if(have[k]==0)continue;
                fin[id[k]]+=uplayer[k]*(mx+1);
                coverbit(id[k],0);
                k=id[k];
                for(int index,i=0;i<d;i++){
                    index=((unsigned)MurmurHash32(&k,4, hashseed+i))%uplen;
                    id[index]^=k;
                    have[index]--;
                    if(have[index]==1)q.push(index);
                }
            }
            for(k=0;k<len;k++){
                if(askbit(k)){
                    last++;
                }
            }
            printf("fin %d:%d/%d/%d\n",level,last,before,len);
        }
        void check(){
            
            for(int k=0;k<len;k++)printf("%d ",ask(k));
            puts("");
            for(int k=0;k<len*bit/8+8;k++)printf("%d ",counter[k]);
            puts("");

        }
    }layer[5];
    int w,d,hashseed;bool unzip=0;
    int*counter;int len;
public:
    BIT_C(int _w,int _d,int _hashseed=1000){
        _w*=8;
        w=_w;
        d=_d;
        hashseed=_hashseed;
        layer[0]=Layer(w,4,3,hashseed,&layer[1],0);
        layer[1]=Layer(w,4,3,hashseed*2,&layer[2],1);
        layer[2]=Layer(w,4,3,hashseed*3,&layer[3],2);
        layer[3]=Layer(w,2,3,hashseed*4,&layer[4],3);
        layer[4]=Layer(w,2,3,hashseed*5,NULL,4);
    }
	void Insert(const char* str){
		int g=0,index[MAX_HASH_NUM];
		for (int i = 0; i < d; i++){
			index[i] = ((unsigned)MurmurHash32(str, KEY_LEN, hashseed + i)) % layer[0].Len();
			g =((unsigned) MurmurHash32(str, KEY_LEN, hashseed + i + d)) % 2;
			if (g == 0){
                layer[0].Insert(index[i],1);
			}else{
                layer[0].Insert(index[i],-1);
			}
		}
	}
	int Query(const char* str){
        if(unzip==0){
            layer[0].Fin(counter,len);
            unzip=1;
        }
		int temp,g;
		int res[MAX_HASH_NUM],index[MAX_HASH_NUM];
		for (int  i=0;i<d;i++){
			index[i] = ((unsigned)MurmurHash32(str, KEY_LEN, hashseed + i)) % len;
			temp = counter[index[i]];
			g = ((unsigned)MurmurHash32(str, KEY_LEN, hashseed + i + d)) % 2;
			res[i] = (g == 0 ? temp : -temp);
		}
		sort(res, res + d);
		if (d % 2 == 0)
			return ((res[d / 2] + res[d / 2 - 1]) / 2);
		else
			return (res[d / 2]);
	}
    void check(){
        // printf("%d %d %d %d %d\n",mem1, mem2, mem3, mem4, mem5);
        // layer[0].check();
        // layer[1].check();
        // layer[2].check();
        // layer[3].check();
        // layer[4].check();
    }
};