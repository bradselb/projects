
// idea:
// create a function to be passed to the list member function, 
// 'foreach_item_call_fctn()'. The function defined here accumulates 
// basic statistics that can be used to calulate the usual and customary
// descriptive summary statistics. Context is stored in an instance of the 
// accompanying data structure. 


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


int accumulate_depth_statistics(void* item, void* ctx)
{
    int rc;

    rc = -1;

    return rc;
}
