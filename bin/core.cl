__kernel void getFactors (int number, global int* result)
{
	int currentID = get_global_id(0);
	if (number % (currentID + 1) == 0)
		result[currentID] = number / (currentID + 1);
	else
	    result[currentID] = 0;
}