// Project      : d_pcl
// File         : reading_pcd.cpp
// Author       : bss
// Creation Date: 2015-01-29
// Last modified: 2015-01-30, 17:57:39
// Description  : read pcd from file
// 

#include <stdio.h>
#include <string>
#include <ros/ros.h>
#include <ros/package.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>

// print usage of this node
void Usage();

int main(int argc, char** argv)
{
    printf("Init\n");

    std::string package_path = ros::package::getPath("d_pcl");
    // pcd file path
    std::string pcd_path = "";
    std::string pcd_name = "";

    int rate_Hz = 5;

    if (argc <= 1)
    {
        Usage();
        return 2;
    }
    for (int i = 1; i < argc; i++)  // 检查命令行参数
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            Usage();
        }
        else if (argv[i][0] != '-')
        {
            pcd_name = argv[i];
            pcd_path = package_path + "/../../../share/d_pcl/"
                    + argv[i];
        }
        else if (strcmp(argv[i], "-r") == 0 ||
                strcmp(argv[i], "--rate") == 0)
        {
            ++i;
            if (i == argc)
            {
                printf("Error: please input rate as a parameter.\n");
                return 2;
            }
            else
            {
                rate_Hz = atoi(argv[i]);
            }
        }
    }

    // load the file
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(
            new pcl::PointCloud<pcl::PointXYZRGB>);
    if (pcl::io::loadPCDFile<pcl::PointXYZRGB>(pcd_path.c_str(), *cloud) == -1)
    {
        printf("Couldn't read file %s\n", pcd_path.c_str());
        return -1;
    }

    // init ros
    ros::init(argc, argv, "test_reading_pcd");
    ros::NodeHandle n;
    ros::Rate rate(rate_Hz);
    ros::Publisher pub = n.advertise<pcl::PointCloud<pcl::PointXYZRGB> >(
            "/pcl/points2", 1);

    // read pcd
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr msg(
            new pcl::PointCloud<pcl::PointXYZRGB>);
    msg->header.frame_id = "test_pcd_" + pcd_name;
    msg->height = cloud->height;
    msg->width = cloud->width;

    for (size_t i = 0; i < cloud->points.size(); ++i)
    {
        msg->points.push_back(cloud->points[i]);
    }

    printf("Init ok\n");
    // publish topic
    while (ros::ok())
    {
        msg->header.stamp = pcl_conversions::toPCL(ros::Time::now());
        pub.publish(msg);
        ros::spinOnce();
        rate.sleep();
    }

    printf("Bye!\n");
    return 0;
}

void Usage()
{
    printf("test_reading_pcd node in d_pcl\n");
    printf("Usage: rosrun d_pcl test_reading_pcd \n");
    printf("[OPTION] SOURCE\n");
    printf("读入一个pcd文件(SOURCE)，并发送\n");
    printf("不要尝试过于不合法的输入\n");
    printf("SOURCE: input file name(put it in ui/d_pcl).\n");
    printf("-h,--help: print help message.\n");
    printf("-r,--rate: sending rate, in Hz.\n");
    printf("\n");
    printf("example:\n");
    printf("rosrun d_pcl test_reading_pcd test.pcd\n");
}

