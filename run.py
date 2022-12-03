import subprocess
import math
import time
import requests
import sys
import argparse
import openpyxl
from openpyxl.styles import PatternFill, Border, Side, Alignment, Protection, Font

parser = argparse.ArgumentParser()
parser.add_argument('-b', '--back', action='store_true', help='need to run program in nohup mode')
parser.add_argument('-type', help='para for Choose_Ske.h',default=0)
args = parser.parse_args()

def change_para(cell_value):
	buffer = ""
	with open(file="./include/params.h",mode='r') as fb:
		buffer = fb.readlines()
	with open(file="./include/params.h",mode='w') as fb:
		for line in buffer:
			if "mem[6]" in line: 
				fb.write("const int mem[6]= {0, " + cell_value + "};\n")
			else: 
				fb.write(line)

if args.back:
	filename = ["caida","campus"]
	filename += ["zipf"+"{:.1f}".format(i/10) for i in range(0,10+1)]
	dataset = ["/share/CAIDA_2018/00.dat", "/share/datasets/campus/campus.dat"]
	dataset += ["/share/zipf_2022/zipf_{:.1f}.dat".format(i/10) for i in range(0,10+1)]

	cnt = 0
	for name,d_set in zip(filename, dataset):#change dataset	
		if name not in ['caida','campus','zipf0.3','zipf0.8']: continue

		path = "/root/bit_experi/{0}_flow_result_32.xlsx".format(name)
		print("target file: ", path)
		#path = "/root/bit_experi/{0}_cm_ver0_changebit_result.xlsx".format(name)
		workbook = openpyxl.load_workbook(path)
		sheet_AAE = workbook["AAE_compare"]
		sheet_ARE = workbook["ARE_compare"]
		sheet_of = workbook["overflow_for_each_layer"]
		workbook.close()

		to_col = {1:'B', 2:'C', 3:'D', 4:'E', 5:'F', 6:'G', 7:'H', 8:'I', 9:'J', 10:'K'}
		for struct_type in range(1,8+1):#change data structure
			print()

			#change ara.h
			if struct_type in range(1,3+1):
				mem_allocate = sheet_AAE['B' + str(struct_type+1)].value
				print(mem_allocate)
				change_para(mem_allocate)
			
			#delete outdated file
			command = "rm sketch{}.csv".format(struct_type)
			print(command)
			s=subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
	
			#compile
			print("make output")
			s=subprocess.run("make", stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
			print(s.stdout.decode("utf8"))
			print(s.stderr.decode("utf8"))

			#run program
			print("main output")
			command = "./main {} {} {}".format(struct_type,d_set,0) #parameter(1, 2, 3) = (struct_type, dataset, mxtime)
			print(command)

			res=[]
			s=subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
			buffer = s.stdout.decode("utf8")
			#print(buffer)
			for line in buffer.split("\n"):
				for target in ["fin", "round", "memory used"]:
					if  target in line:
						print(line)
						line = line.replace("\033[34m","")
						line = line.replace("\033[0m","")
						res.append( line )		

			print()
			#write output to excel file
			memory_used=""
			cell_start_round = 7
			cell_start_fin = 7
			for line in res:
				column = to_col[struct_type]
				if "memory used" in line:
					memory_used = line.split(" ")[-1]

				if "fin" in line:
					#print(cell_start_fin," ",line)
					sheet_of['A'+str(cell_start_fin)].value = memory_used
					sheet_of[ column+str(cell_start_fin) ] = line
					cell_start_fin+=1

				if "round" in line:
					#print(cell_start_round," ",line)
					sheet_AAE['A'+str(cell_start_round)].value = memory_used
					sheet_ARE['A'+str(cell_start_round)].value = memory_used
					are = float(line.split(" ")[1])
					aae = float(line.split(" ")[2])
					sheet_AAE[ column+str(cell_start_round) ].value = aae
					sheet_ARE[ column+str(cell_start_round) ].value = are
					cell_start_round+=1

			for col in "ABCD":
				if col=='A': sheet_of.column_dimensions[col].width = len(sheet_of['A6'].value) * 1.1
				else: sheet_of.column_dimensions[col].width = len(sheet_of['B7'].value) * 1.1
				
				for row in range(7,cell_start_round):
					sheet_of[col+str(row)].alignment = Alignment(horizontal='center')
					sheet_of[col+str(row)].number_format = '0.000000'

			for col in "ABCDEFGHI":
				for row in range(7,cell_start_round):
					sheet_AAE[col+str(row)].alignment = Alignment(horizontal='right')
					sheet_ARE[col+str(row)].alignment = Alignment(horizontal='right')
					sheet_AAE[col+str(row)].number_format = '0.000000'
					sheet_ARE[col+str(row)].number_format = '0.000000'
		
			workbook.save(filename = path)
		
		cnt += 1
	
else:
	#simple run
	s=subprocess.Popen("rm sketch{}.csv".format(args.type), stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
	
	print("make output")
	s=subprocess.run("make", stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
	print(s.stdout.decode("utf8"))
	print(s.stderr.decode("utf8"))
	
	print("main output")
	res=""
	s=subprocess.Popen("./main {}".format(args.type), stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
	while s.poll() is None:
		line=s.stdout.readline().decode("utf8")
		print(line,end="")
		if "fin" in line or "mem" in line or "round" in line:
			res+=line
	
	print("res")
	print(res)

	