// define a basic statistics data structure. 
// The structure has fields for a few very fundamental
// statistics - those necessary to calculate the usual and customary
// basic descriptive statistics - mean and variance for example. 
// there are also two fields that can be used to represent a 
// frequency table or histogram. 
// bin_freq is a dynamically allocated array of 'bin_count' elements
// each entry in the array represents a bin. 
// the content of each element in the array is the frequency, 
// number of observations that fall in that bin. The index into
// the array is some integral value that represents the range of
// values in the bin.  




struct basic_statistics
{
    int sum;
    int sum_squared;
    int n;
    int bin_count;
    int* bin_freq;
};

struct basic_statistics* allocate_basic_statistics(void)
{
}

void free_basic_statistics(struct basic_statistics* stats)
{
}
