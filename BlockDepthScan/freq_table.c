struct depth_statistics
{
    int freq[10]; // depth is index, value at index is freq of that depth
    int sum_depth;
    int sum_squared;
    int n;
};


int accumulate_depth_statistics(void* item, void* ctx)
{
    int rc;

    rc = -1;

    return rc;
}
