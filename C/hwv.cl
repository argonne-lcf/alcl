__kernel void hello_world() {
    const int world_rank = get_global_id(0);
    const int work_size =  get_global_size(0);
    const int local_rank = get_local_id(0);
    const int local_size = get_local_size(0);
    const int group_rank = get_group_id(0); 
    const int group_size = get_num_groups(0); 
    printf("Hello world : World rank/size: %d / %d. Local rank/size: %d / %d  Group rank/size: %d / %d \n", world_rank, work_size, local_rank, local_size, group_rank, group_size);
};

