#include <stdio.h>
int int_val1 = 1, int_val2 = 2, int_val3 = 3, int_val4 = 4, int_val5 = 5;
float float_val = 22.22;
uint64_t t = 33;
int pass_int_function(int int_val1, int int_val2, int int_val3, int int_val4, int int_val5){
	return (int_val1 + int_val2 + int_val3 + int_val4 + int_val5);
}

float pass_float(float float_val){
	return (float_val);
}
uint64_t pass_uint64_t(uint64_t t){
	return (t);
}
void reset_handler(void)
{
	int temp1;
	float temp2;
	uint64_t temp3;
	temp1 = pass_int_function(int_val1, int_val2, int_val3, int_val4, int_val5);
	temp2 = pass_float(float_val);
	temp3 = pass_uint64_t(t);
	while (1)
		;
}
