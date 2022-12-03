class Sketch {
public:
	virtual void Insert(const char* str)=0;
	virtual int Query(const char* str)=0;
	virtual void check(){
		puts("you didn't difine this check");
	};
	virtual void temp_insert(int pos,int k){ printf("base\n"); }
	virtual int layer0_len() {return -1;}
};