#include <stdafx.h>
#include <iostream>
#include <fstream>
using namespace  std;
class wavfile
{
public:
	unsigned short cahnnel;			// 声道数
	unsigned int frequency;			// 采样频率
	unsigned int byte_rate;			// 比特率，frequency * cahnnel * bit_per_sample /8
	unsigned short bit_per_sample;	// 采样位数 8位或者16 24等
	unsigned int file_size;			// 文件大小
	unsigned long data_size;		// 实际数据文件大小（字节形式）
	unsigned char *original_data;	// 实际存储的数据 （从字节形式进行转化)
	double *norm_data;				// 归一化数据
	double *data;					// default output is mono data
	unsigned long len;				// 实际点数,data_size/(bit_per_sample/8) /channel
	double duration;				// 持续时间(second)
	bool is_open;

	wavfile();
	~wavfile();
	int read(std::string file_neme);		// read the wav file
	void show();					// show wav file message
	int write(std::string file_name);	// write data to wav file
private:

};

wavfile::wavfile()
{
	// 构造函数 相当于初始化

}

wavfile::~wavfile()
{
	// 析构函数 释放资源
	this->is_open = false;
}

int wavfile::read(std::string file_neme)
{
	ifstream fp;																	// 输入时 ifstream
	fp.open(file_neme, ios::in | ios::binary);										// 读取文件 try except
	if (!fp.is_open()) {
		std::cout << "Can not open it." << endl;
		fp.close();
		return false;
	}
	fp.seekg(0, ios::end);								// seek get
	this->file_size = fp.tellg();						// 从后往前，输出的是整个文件的大小(字节形式)
	fp.seekg(0x16);
	fp.read((char*)&this->cahnnel, 2);					// 声道
	fp.seekg(0x18);										// offset 24 is num channels
	fp.read((char *)&this->frequency, 4);				// 采样频率
	fp.read((char*)&this->byte_rate, 4);				// 比特率
	fp.seekg(0x22);
	fp.read((char*)&this->bit_per_sample, 2);			// 采样位数	
	fp.seekg(0x28);
	fp.read((char*)&this->data_size, 4);

	this->original_data = new unsigned  char[this->data_size];// 开辟数据空间接收

	fp.seekg(0x2c);										// 44开始是data域的东西
	fp.read((char*)this->original_data, this->data_size);

	this->norm_data = new double[this->data_size / 2];
	for (unsigned long win_len = 0; win_len < this->data_size; win_len = win_len + 2) {
		long temp_value = 0;
		if ((this->original_data[win_len + 1] >> 7) == 1) {								// 判断正负数,x86系统中是小端模式，低位低地址，高位高地址，两字节
			temp_value = this->original_data[win_len] | (this->original_data[win_len + 1] << 8) - 65536;
		}
		else {
			temp_value = this->original_data[win_len] | this->original_data[win_len + 1] << 8;			// 正数
		}
		this->norm_data[win_len / 2] = (double)temp_value / 32768.0;									// 归一化

	}

	// output data

	if (this->cahnnel > 1) {
		this->data = new double[this->data_size / 4];
		long count = 0;
		for (long win_len = 0; win_len < this->data_size / 4; win_len += 2) {
			this->data[count++] = (this->norm_data[win_len] + this->norm_data[win_len + 1]) / 2.0;
		}
	}
	else
		this->data = this->norm_data;
	fp.close();
	this->len = this->data_size / 4;
	this->duration = this->len / this->frequency;
	this->is_open = true;
	return true;
}

void wavfile::show()
{
	if (this->is_open) {
		std::cout << "The Wav file message:" << endl
			<< "Frequency	: " << this->frequency << " Hz" << endl
			<< "Bit_per_sample	: " << this->bit_per_sample << " bps" << endl
			<< "Size	: " << this->len << " samples" << endl
			<< "Duration: " << this->duration << " seconds" << endl;
	}
	else
		std::cout << "Have not open any wav file." << endl;
}

int wavfile::write(std::string file_name)
{
	ofstream fp;
	fp.open(file_name, ios::out || ios::binary);
	//fp.write()
	return 0;
}
