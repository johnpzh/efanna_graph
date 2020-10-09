//
// Created by Zhen Peng on 09/21/2020.
//

#include <efanna2e/index_graph.h>
#include <efanna2e/index_random.h>
#include <efanna2e/util.h>


void load_data(char* filename, float*& data, unsigned& num,unsigned& dim){// load data with sift10K pattern
  std::ifstream in(filename, std::ios::binary);
  if(!in.is_open()){std::cout<<"open file error"<<std::endl;exit(-1);}
  in.read((char*)&dim,4);
  std::cout<<"data dimension: "<<dim<<std::endl;
  in.seekg(0,std::ios::end);
  std::ios::pos_type ss = in.tellg();
  size_t fsize = (size_t)ss;
  num = (unsigned)(fsize / (dim+4));
//  num = (unsigned)(fsize / (dim+1) / 4);
  uint64_t data_length = static_cast<uint64_t>(num) * static_cast<uint64_t>(dim);
//  uint64_t data_bytes = sizeof(float) * static_cast<uint64_t>(num) * static_cast<uint64_t>(dim);
  data = new float[data_length];
//  data = new float[num * dim * sizeof(float)];

  in.seekg(0,std::ios::beg);
  std::vector<char> buffer(dim);
  float *data_base = data;
  for(size_t i = 0; i < num; i++){
    in.seekg(4,std::ios::cur);
    in.read(buffer.data(), dim);
    for (unsigned d = 0; d < dim; ++d) {
        *(data_base + d) = buffer[d];
    }
    data_base += dim;
//    in.read((char*)(data+i*dim),dim*4);
  }
  in.close();
}
int main(int argc, char** argv){
  if(argc!=8){std::cout<< argv[0] <<" data_file save_graph K L iter S R"<<std::endl; exit(-1);}
  float* data_load = NULL;
  unsigned points_num, dim;
  printf("Loading %s...\n", argv[1]);
  load_data(argv[1], data_load, points_num, dim);
  char* graph_filename = argv[2];
  unsigned K = (unsigned)atoi(argv[3]);
  unsigned L = (unsigned)atoi(argv[4]);
  unsigned iter = (unsigned)atoi(argv[5]);
  unsigned S = (unsigned)atoi(argv[6]);
  unsigned R = (unsigned)atoi(argv[7]);
  //data_load = efanna2e::data_align(data_load, points_num, dim);//one must align the data before build
  efanna2e::IndexRandom init_index(dim, points_num);
  efanna2e::IndexGraph index(dim, points_num, efanna2e::L2, (efanna2e::Index*)(&init_index));

  efanna2e::Parameters paras;
  paras.Set<unsigned>("K", K);
  paras.Set<unsigned>("L", L);
  paras.Set<unsigned>("iter", iter);
  paras.Set<unsigned>("S", S);
  paras.Set<unsigned>("R", R);

  auto s = std::chrono::high_resolution_clock::now();
  index.Build(points_num, data_load, paras);
  auto e = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = e-s;
  std::cout <<"Time cost: "<< diff.count() << "\n";

  index.Save(graph_filename);

  return 0;
}
