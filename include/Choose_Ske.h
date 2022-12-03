#include "Sketch.h"
#include "SAC.h"
#include "SalsaCM.h"
#include "SalsaCount.h"
#include "CM_Sketch.h"
//#include "CU_Sketch.h"
//#include "C_Sketch.h"
//#include "A_Sketch.h"
#include "PCM_Sketch.h"
#include "StingyCM.h"
#include "BIT_CM.h"
#include "BIT_CM_ver2.h"
#include "BIT_CM_ver3.h"
#include "BIT_Count.h"
Sketch* Choose_Sketch(uint32_t w, uint32_t d, uint32_t hash_seed = 1000,int id=10){
	switch (id){
		case 1: sketch_name = "BIT_CM_ver2";      return new BIT_CM_ver2(w,d,hash_seed);
		case 2: sketch_name = "BIT_CM";           return new BIT_CM(w,d,hash_seed);
		case 3: sketch_name = "BIT_C";            return new BIT_C(w,d,hash_seed);
		case 4: sketch_name = "StingyCM";         return new StingyCM(w,d,hash_seed);
		case 5: sketch_name = "PCM_Sketch";       return new PCM_Sketch(w,d,hash_seed);
		case 6: sketch_name = "SAC";              return new SAC(w,d,hash_seed);
		case 7: sketch_name = "CM_Sketch";        return new CM_Sketch(w,d,hash_seed);
		case 8: sketch_name = "SalsaCM";          return new SalsaCM(w,d,hash_seed);

		case 9: sketch_name = "BIT_CM_ver3";      return new BIT_CM_ver3(w,d,hash_seed);
	}
	return NULL;
}
