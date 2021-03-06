// Spatial_visual.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


/*
*  Copyright (c) 2008 Dustin Spicuzza <dustin@virtualroadside.com>
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of version 2.1 of the GNU Lesser General Public
*  License as published by the Free Software Foundation.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <string>
#include <regex>

#include <string>
#include <ctime>
//#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <stdio.h>
#include "../Spatial_visual/lib/RStarTree.h"
#include "../Spatial_visual/lib/SpatialVector.h"
//typedef  unordered_map<string, vector<double>> ldVectorMap= VisualVector::loadLdVectorMap(HD_file);
#include <set>
#define SAMPLE_SIZE 1445
#define EDTHRESHOLD 1.0  //0.1
#define DELTA 0.0001
#include <iostream>
#include <unordered_map>

#include <stdlib.h>
#include <chrono>
using namespace std;

//VisualalVector::loadHdVectorMap(HD_file);
//#ifdef RANDOM_DATASET
typedef RStarTree<double, 2, 50, 100> 			RTree;
//#else
//	typedef RStarTree<std::string, 2, 2, 3> 	RTree;
//#endif

typedef RTree::BoundingBox			BoundingBox;


BoundingBox bounds(string image_name, double min_x, double min_y, double max_x, double max_y)
{
	BoundingBox bb;

	bb.image_name = image_name;
	bb.edges[0].first = min_x;
	bb.edges[0].second = max_x;

	bb.edges[1].first = min_y;
	bb.edges[1].second = max_y;

	return bb;
}


struct Visitor {
	int count;
	bool ContinueVisiting;

	Visitor() : count(0), ContinueVisiting(true) {};

	void operator()(const RTree::Leaf * const leaf)
	{

		cout << "Visiting " << count << std::endl;
		//#if defined( RANDOM_DATASET )
		//		//std::cout << "Visiting " << count << std::endl;
		//#elif defined( GUTTMAN_DATASET )
		//		std::cout << "#" << count << ": visited " << leaf->leaf << " with bound " << leaf->bound.ToString() << std::endl;
		//#else
		//		#error "Undefined dataset"
		//#endif
		count++;
	}
};


struct SQResultTracker {

	int node_count;
	vector<string> matched;


	SQResultTracker() :node_count(0) {};

};




set<string> getRandomSample(string file_name, int size) {
	std::set<std::string> samples;
	ofstream outfile3;
	outfile3.open("sample_img.txt");
	cout << file_name << endl << endl;
	int number_of_lines = size;

	// a vector to hold all the indices: 0 to number_of_lines
	std::vector<int> line_indices(number_of_lines);
	std::iota(begin(line_indices), end(line_indices), 0); // init line_indices

														  // C++11 random library (should be preferred over rand()/srand())
	std::random_device r;
	std::seed_seq seed{ r(), r(), r(), r(), r(), r(), r(), r() };
	std::mt19937 eng(seed);

	// shuffle the line_indices:
	std::shuffle(begin(line_indices), end(line_indices), eng);

	int number_of_lines_to_select = SAMPLE_SIZE;
	assert(number_of_lines_to_select <= number_of_lines);

	std::string line;
	std::ifstream data_file(file_name);
	if (data_file.is_open()) {
		
	int line_number = 0;
	while (std::getline(data_file, line)) {
		for (int i = 0; i < number_of_lines_to_select; ++i) {
			if (line_number == line_indices[i]) {
				std::string firstWord = line.substr(0, line.find("\t"));

				cout << " The word is: " << firstWord << endl;
				cout << endl;
				outfile3 << firstWord << endl;
				samples.insert(firstWord);
				std::cout << firstWord << '\n';
			}
		}
		++line_number;
		
	}
	data_file.close();
	outfile3.close();
}
		else {
			cout << "File not found :" << file_name << endl;
			
		}

		return samples;
}
set<string> getRandomSamples(string file_name) {
	std::set<std::string> samples;

	cout << file_name << endl << endl;
	int c = 0;
	while (samples.size() < SAMPLE_SIZE) {
		ifstream data_file(file_name);

		if (data_file.is_open()) {
			string line;
			int nlines = 1;
			int linecount = 0;

			while (getline(data_file, line)) // && samples.size() < SAMPLE_SIZE)
			{

				// TODO: uncomment the if statement in future
				//if ((rand() % nlines) == 0) // break ;
				// {
				//regex ws_re("\\s+");
				//vector<std::string> result{
				//		sregex_token_iterator(line.begin(), line.end(), ws_re, -1), {}
				//};
				//string search_file_name =  regex_replace(result[0], regex("USC_Dataset"), "");

				//samples.insert(search_file_name);
				samples.insert(line);
				// cout<< line <<"    " <<nlines << endl;
				nlines++;
				// }
				linecount++;
			}


			data_file.close();
		}
		else {
			cout << "File not found :" << file_name << endl;
			break;
		}

	}
	return samples;
}

bool is_number(const std::string& s)
{

	std::stringstream sstr(s);
	double d;
	return bool(sstr >> d);
}

void buildandsearch(int HD, string HD_file, float delta)
{

	auto start = chrono::steady_clock::now();
	RTree tree;
	string dash = "-------------------------------------------------------------------";
	cout << dash << HD << "\t" << HD_file << "\t" << delta << dash << endl;

	string file_name = "Data/USC_MBR_Appraoch2.txt";
	string::size_type  sz;

	ifstream data_file(file_name);
	string line;

	SpacialVector::loadSpacialMap(file_name);
	VisualVector::loadHdVectorMap(HD_file);

	float id = 0;
	if (data_file.is_open())
	{
		while (getline(data_file, line))
		{

			regex ws_re("\\s+");
			
			vector<std::string> result{
				sregex_token_iterator(line.begin(), line.end(), ws_re, -1),{}
			};
			//result.resize(3361706);
			if (is_number(result[3]) && is_number(result[4]), is_number(result[5]), is_number(result[6])) {
				BoundingBox bb = bounds(result[0].replace(0, 11, ""), stod(result[3], &sz), stod(result[4], &sz),
					stod(result[5], &sz), stod(result[6], &sz));
				tree.Insert(id, bb);
				id++;
			}
			else {
				cout << "string does not match number" << endl;
			}
		}
		data_file.close();
		cout << "Done loading R* tree" << endl;

	}
	else cout << "File not found";


	cout << "Loading Samples....... " << endl;

	//set<string> samples = getRandomSample("Dataset/RMAC.tsv",24345);
	//set<string> samples = getRandomSamples("Data/random_sample_o.txt");
	std::set<std::string> samples;
	string file_name_sample = "sample_img.txt";
	//ifstream data_file_sample(file_name);
	std::string lines;
	//std::vector res;
	std::ifstream data_file_sample(file_name_sample);


	if (data_file_sample.is_open()) {

		int line_number = 0;
		while (std::getline(data_file_sample, lines)) {
			std::istringstream iss(lines);
			
			samples.insert(lines);
			cout << lines << endl;

		}
		data_file_sample.close();
	}
	else {
		cout << "File not found :" << file_name_sample<< endl;

	}
	cout << "Loading Samples " << endl;
	ofstream outfile1;
	outfile1.open("result_ed"+ std::to_string(delta) +".txt");
	ofstream all_images;
	all_images.open("query_images.txt");
	ofstream matched;
	matched.open("matched_images.txt");
	vector< pair<string, pair<string, double> > > imagemap;
	for (string s : samples) {

		SQResultTracker tracker;
		string search_file_name = s;

		spacialObject value = SpacialVector::getSpacialMapValue(s);

		BoundingBox bound = bounds(search_file_name, value.x_min - DELTA, value.y_min - DELTA, value.x_max + DELTA, value.y_max + DELTA);
		SQResultTracker rRracker = tree.SearchQuery(tracker, bound);
		vector<double> image_vector = VisualVector::getHdVectorMapValue(search_file_name);

		int i = 0;
		int j = 0;
		
		double max = 0;
		string org_img="";
		string max_img="_";
		//double ed;
		
		
		for (string image : rRracker.matched) {
			vector<double> smatch_vector = VisualVector::getHdVectorMapValue(image);
			all_images << s << "\t" << image << endl;

			//vector<pair<string, double>> imagemap;
			//vector<vector<int>> matrix(imagemap, vector<int>(CC));
			if (smatch_vector.size() >0) {

				double ed = VisualVector::vectorsDistance(image_vector, smatch_vector);
				if (ed <= delta & max <= ed) {
					
					i++;
					
						max = ed;
						org_img = search_file_name;
						max_img = image;


						//imagemap.push_back(make_pair(search_file_name, make_pair(image, ed)));
						cout << s << "\t" << image << "maxsize"<<max <<"\t"<<ed<< endl;
						matched << s << "\t" << image << "\t" << ed << endl;
						matched << "number of images match: " << i << endl;
					

				}
				
			}
			
		}
		
		//ofstream outfile2;
		//outfile2.open("notfound.txt");
		if (max_img != "_") {
			//imagemap.push_back(make_pair(search_file_name, make_pair(max_img, ed)));
			//outfile1 << imagemap[i].first << "\t" << imagemap[i].second.first << "\t " << imagemap[i].second.second << endl;
			 outfile1 <<org_img << "\t" <<max_img << "\t " << max<< endl;
			 cout << org_img << "\t" << max_img <<"\t"<< max << endl;
		}
		else {
			cout << org_img << endl;
			//outfile2 << org_img << endl;
		}
	}
	
	//outfile2.close();
	int s = imagemap.size();
	//ofstream outfile1;
	//outfile1.open("result.txt");
	
	//outfile << "sampled image" << " ," << "max_ed_image" << " ," << "high_d_distance" << " ," << "3d_distance" << " ," << "6d_distance" << " ," << "9d_distance" << " ," << "12d_distance" << " ," << "15d_distance" << endl;
	
	for (int i = 0; i<s; i++)
	{
		cout << imagemap[i].first << "\t" << imagemap[i].second.first<< "\t " << imagemap[i].second.second << endl;
		//outfile1 << imagemap[i].first << "\t" << imagemap[i].second.first << "\t " << imagemap[i].second.second << endl;
	}
	outfile1.close();
	all_images.close();
	matched.close();
	auto end = chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	cout << elapsed.count() << endl;

	
}

void lowerboundsearch(vector <string> samp_pair, string output_file_name) {
	for (int i = 3; i <= 15; i =i+3) {
		cout << "start" << endl;
		string loadvector= "result_" + std::to_string(i) + "_RMAC.tsv";
		cout << loadvector << endl;
		VisualVector::loadLdVectorMap("Dataset/"+loadvector);
		ofstream outfile4;
		outfile4.open(output_file_name+"_"+ std::to_string(i)+".tsv");
		cout << output_file_name + "_" + std::to_string(i) + ".tsv" << endl;
		outfile4 << "image_name" << "\t" << std::to_string(i) + "D_delta" << endl;
		cout << "ending" << endl;
		for (string line : samp_pair) {
			std::istringstream iss(line);
			std::vector<std::string> results(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());

			vector<double> search_image_vec = VisualVector::getLdVectorMapValue(results[0]);
			vector<double> match_image_vec = VisualVector::getLdVectorMapValue(results[1]);
			double ed = VisualVector::vectorsDistance(search_image_vec, match_image_vec);
			//cout << results[0] << results[1] << ed << endl;
			outfile4 << results[0] << "\t" << ed << endl;

		}
	}
}
	

int main(int argc, char ** argv) {
	//set<string> samples = getRandomSample("Dataset/RMAC.tsv",24345);


	string HD[3] = { "Data/feature.csv", "Dataset/RMAC_512D.tsv", "Dataset/result_3_RMAC.tsv" };
	string lower[5] = { "Dataset/result_3_RMAC.tsv" ,"Dataset/result_6_RMAC.tsv" ,"Dataset/result_9_RMAC.tsv" ,"Dataset/result_12_RMAC.tsv" ,"Dataset/result_15_RMAC.tsv" };
	int HDD[3] = { 4096, 512, 3 };
	float delta[4] = { 0.8,0.9,1.1,1.0};
	//int i =1;
	int d = 5;

	vector <string> sample_op_pair;
	string file_name = "result_ed1.txt";
	std::string line;
	//std::vector res;
	std::ifstream data_file(file_name);


	if (data_file.is_open()) {

		int line_number = 0;
		while (std::getline(data_file, line)) {
			std::istringstream iss(line);
			sample_op_pair.push_back(line);
			std::vector<std::string> results(std::istream_iterator<std::string>{iss},
				std::istream_iterator<std::string>());

			//result.push_back(make_pair(line[1], make_pair(line[2],line[3])));
				//playerInfoVector.push_back(token);
				//std::cout << token << '\n';
			//cout << line << endl;
					//std::string firstWord = line.substr(0, line.find("\t"));

					//std::string second = line.substr(line.find("\t"), line.find("\t"));
					//std::string third = line.substr(line.find("\t"), line.find("\t"));
					//cout << " The word is: " << firstWord << second << endl;
			cout << endl;
			//result.push_back(make_pair(firstWord, second));
			//std::cout << firstWord << '\n';


	//++line_number;

		}
		data_file.close();
	}
	else {
		cout << "File not found :" << file_name << endl;

	}
	//auto start = chrono::steady_clock::now();
	for (int d = 0; d < 3; d++) {
		//int i = 1;
			//	for (int i = 0; i < 1; i++) {

		cout << HDD[1] << HD[1] << delta[d];
		buildandsearch(HDD[1], HD[1], delta[4]);
		//string output_filename = "lowerdim_ed" + delta[d];
	//lowerboundsearch(sample_op_pair, "ld_ed1");

	}
	//auto end = chrono::steady_clock::now();
	//std::chrono::duration<double> elapsed = end - start;
	//cout << elapsed.count() << endl;

	//}
}
