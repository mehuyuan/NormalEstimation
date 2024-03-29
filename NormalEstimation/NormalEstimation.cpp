#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/features/normal_3d.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <math.h>

/* 解决vtk编译出现的错误:
 * Generic Warning: In C:\Build\3rdParty\x86\VTK-7.0.0\Rendering\Core\vtkActor.cxx, line 43
 * Error: no override found for 'vtkActor'.
 */
#include <vtkAutoInit.h>  
VTK_MODULE_INIT(vtkRenderingOpenGL);

int main()
{
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
	//// 从文件导入点云
	//// 激光传感器、摄像头等测得的点云必须经过滤波处理，否则可能无法直接计算出法向量
	//if (pcl::io::loadPCDFile<pcl::PointXYZ>("Ground_PCD.pcd", *cloud) == -1)
	//{
	//	PCL_ERROR("Couldn't read file test_pcd.pcd\n");
	//	return(-1);
	//}
	// 创建点云
	cloud->width = 950;
	cloud->height = 1000;
	cloud->points.resize(cloud->width * cloud->height);
	cloud->is_dense = true;
	for(int i{};i<cloud->height;++i)
		for (int j{}; j < cloud->width; ++j)
		{
			cloud->points[i*cloud->width + j].x = 30 * cos(j * M_PI / (cloud->width - 1));
			cloud->points[i*cloud->width + j].y = 30 * sin(j * M_PI / (cloud->width - 1));
			cloud->points[i*cloud->width + j].z = 0.1 * i;
		}

	//////// Create a set of indices to be used.
	//////std::vector<int> indices(round(cloud->size() / 100));
	//////for (int i{}; i < cloud->height / 10; ++i)
	//////	for (int j{}; j < cloud->width / 10; ++j)
	//////		indices[i*cloud->width / 10 + j] = 10 * i*cloud->width + 10 * j;

	// Create the normal estimation class, and pass the input dataset to it
	pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> ne;
	ne.setInputCloud(cloud);

	//////// Pass the indices
	//////boost::shared_ptr<std::vector<int>> indicesptr(new std::vector<int>(indices));
	//////ne.setIndices(indicesptr);

	// Create an empty kdtree representation, and pass it to the normal estimation object.
	// Its content will be filled inside the object, based on the given input dataset (as no other search surface is given).
	pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>());
	ne.setSearchMethod(tree);

	// Output datasets
	pcl::PointCloud<pcl::Normal>::Ptr cloud_normals(new pcl::PointCloud<pcl::Normal>);

	// Use all neighbors in a sphere of radius 3cm
	ne.setRadiusSearch(0.5);

	// Compute the features
	ne.compute(*cloud_normals);

	// Concatenate the XYZ and normal fields*
	//pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointNormal>);
	//pcl::concatenateFields(*cloud, *cloud_normals, *cloud_with_normals);

	/*图像显示*/
	// 创建显示对象
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
	// 设置背景色
	viewer->setBackgroundColor(0, 0, 0.7);
	// 设置点云颜色，此处为单一颜色设置
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> single_color(cloud, 0, 255, 0);
	// 添加需要显示的点云数据
	viewer->addPointCloud<pcl::PointXYZ>(cloud, single_color, "sample cloud");
	// 设置点显示大小
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud");
	// 添加需要显示的点云法向
	// cloud为原始点云模型，normal为法向信息，10表示需要显示法向的点云间隔，即每10个点显示一次法向，5表示法向长度。
	viewer->addPointCloudNormals<pcl::PointXYZ, pcl::Normal>(cloud, cloud_normals, 10, 5.0f, "normals");

	while (!viewer->wasStopped())
	{
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}

	return(1);
}