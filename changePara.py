import sys

begin=0
save=0
content=""

with open(file="include/params.h",mode="r") as fb:
	while True:
		line = fb.readline()
		if not line:break
		
		begin=line.find("//memory")
		if begin != -1:
			save=fb.tell()
			fb.seek(0)
			content=fb.read(save)
			break

para=[200000, 60000, 10000, 1000, 1000]
para[0]=int(sys.argv[1])

with open(file="include/params.h",mode="w") as fb:
	fb.write(content)

	prefix = "const int mem[6]= {0, "
	command = "{}, {}, {}, {}, {}".format(para[0],para[1],para[2],para[3],para[4]) 
	suffix = "};"
	fb.write(prefix + command + suffix)
	
	print(para)
