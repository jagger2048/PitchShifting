// wavread.cpp: 定义控制台应用程序的入口点
#include <stdafx.h>
#include <iostream>
#include <fstream>
#include <typeinfo.h>
using namespace std;
struct wav_struct {
	unsigned short channel_num;		// 声道数
	unsigned int frequency;			// 采样频率
	unsigned int byte_rate;			// 比特率，frequency * channel_num * bit_per_sample /8
	unsigned short bit_per_sample;	// 采样位数 8位或者16 24等
	unsigned int file_size;			// 文件大小
	unsigned int data_size;			// 实际数据大小
	unsigned char *original_data;			// 实际存储的数据
	double *norm_data;				// 归一化数据
	double *data;					// default output is mono data
};
class wavFile
{
public:
	wavFile();
	~wavFile();

private:

};

wavFile::wavFile()
{
}

wavFile::~wavFile()
{
}

int main(void)
{
	// 换电脑时需要更新解决方案(清理+重定向项目)
	const string file_name = "D:\\Project\\effect_c\\audioData\\Long Live.wav";		// 需要使用绝对地址 work-pc
	//const string file_name = "D:\\WorkSpace\\Matlab\\GPE\\Audio Data\\Long Live.wav";		// 需要使用绝对地址 home-pc


	ifstream fp;																	// 输入时 ifstream
	wav_struct wav;
	unsigned char *p;
	fp.open(file_name, ios::in | ios::binary);										// 读取文件 try except
	if (!fp.is_open()) {
		cout << "Can not open it." << endl;
		//exit(0);
	}

	fp.seekg(0, ios::end);						// seek get
	wav.file_size =  fp.tellg();				// 从后往前，输出的是整个文件的大小(字节形式)
	fp.seekg(0x16);
	fp.read((char*)&wav.channel_num, 2);		// 声道
	fp.seekg(0x18);	// offset 24 is num channels
	fp.read((char *)&wav.frequency,4);			// 采样频率
	fp.read((char*)&wav.byte_rate, 4);			// 比特率
	fp.seekg(0x22);
	fp.read((char*)&wav.bit_per_sample, 2);		// 采样位数	
	fp.seekg(0x28);
	fp.read((char*)&wav.data_size, 4);

	wav.original_data = new unsigned  char[wav.data_size];// 开辟数据空间接收
	
	fp.seekg(0x2c);								// 44开始是data域的东西
	cout << "cur1:" << fp.tellg() << endl;
	fp.read( (char*)wav.original_data, wav.data_size );
	cout << "cur2:" << fp.tellg() << endl;
	cout << "data_size(字节):" << wav.data_size << endl;
	cout << sizeof(wav.original_data) << endl;
	cout << "start here" << endl;
	wav.norm_data = new double[wav.data_size/2];
	for (int n = 0; n < wav.data_size; n=n+2) {
		long temp_value = 0;
		if ((wav.original_data[n + 1] >>7) == 1) {									// 这里的判断有误， 需要把0x01换成0x80 或者右移7位
			//printf("%x", (0x8000 | 0x7fff & ((~wav.original_data[n + 1]) << 8) | ~wav.original_data[n] | 0x01) );
			//value =   0x8000 | 0x7fff&( (~wav.original_data[n + 1]) << 8 ) | ~wav.original_data[n] | 0x01 ;// 2字节负数从补码求原码-求法错误
			temp_value = wav.original_data[n] | (wav.original_data[n + 1] << 8 ) - 65536;						// 负数
		}
		else {
			temp_value = wav.original_data[n] | wav.original_data[n + 1] << 8;								// 正数
		}
		wav.norm_data[n / 2]= (double)temp_value / 32768.0;											// 归一化

	}

	// output data

	if (wav.channel_num > 1) {
		wav.data = new double[wav.data_size / 4];
		long count = 0;
		for (long n = 0; n < wav.data_size/4; n+=2) {
			wav.data[count++] = ( wav.norm_data[n] + wav.norm_data[n + 1]) / 2.0;
		}
	}
	else
		wav.data = wav.norm_data;
	cout << "end here" << endl;

	ofstream out;
	out.open("test.txt", ios::out);
	//out << wav.data << endl;
	for (int i = 0; i < wav.data_size / 4; i++) {
		out << wav.data[i] << endl; 
	}
	//out.close;
	system("pause");
	return 0;
}
/*
TODO：
	1. 封装成类，方便调用
	2. 优化双声道数据读取
	3. 使用向量的形式进行操作

	已知BUG：
		1. 所换算出来的值比matlab中的值(浮点)少一半，不知为何，需要重新校验matlab中的解码方式，或者
			自己编写的解码方式，就目前来说影响不大。
	
*/