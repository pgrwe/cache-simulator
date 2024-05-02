#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;
class result{
    public: 
        unsigned long long cache_hits = 0; // cache hits (x)
        unsigned long long accesses = 0; // total memory accesses (y) 
};

class cache_info{
    public:
        double last_used;
        vector<unsigned long long> cache_line;
};

result direct_mapped(string infile_name, int cache_size, int cache_line_size);
result set_associative(string infile_name, int cache_size, int cache_line_size, int associativity);
result fully_associative(string infile_name, int cache_size, int cache_line_size, int replacement_policy);
result set_assoc_no_alloc(string infile_name, int cache_size, int cache_line_size, int associativity);
result set_assoc_prefetch(string infile_name, int cache_size, int cache_line_size, int associativity);
result prefetch_on_miss(string infile_name, int cache_size, int cache_line_size, int associativity);

int main(int argc, char *argv[]){
    
    string infile_name, line, behavior;
    unsigned long long address;
    result temp;

    infile_name = argv[1];
    ofstream outfile(argv[2]);

    int one_kb = 1024;
    // Direct Mapped
    // 1KB
    temp = direct_mapped(infile_name, one_kb, 32);
    outfile << temp.cache_hits << " " << temp.accesses << "; ";
    // 4KB
    temp = direct_mapped(infile_name, 4*one_kb, 32);
    outfile << temp.cache_hits << " " << temp.accesses << "; ";
    // 16KB
    temp = direct_mapped(infile_name, 16*one_kb, 32);
    outfile << temp.cache_hits << " " << temp.accesses << "; ";
    // 32KB
    temp = direct_mapped(infile_name, 32*one_kb, 32);
    outfile << temp.cache_hits << " " << temp.accesses << "; " << endl;

    // Set Associative with LRU
    // Associativity 2
    temp = set_associative(infile_name, 16*one_kb, 32, 2);
    outfile << temp.cache_hits << " " << temp.accesses << "; ";
    // Associativity 4
    temp = set_associative(infile_name, 16*one_kb, 32, 4);
    outfile << temp.cache_hits << " " << temp.accesses << "; ";
    // Associativity 8
    temp = set_associative(infile_name, 16*one_kb, 32, 8);
    outfile << temp.cache_hits << " " << temp.accesses << "; ";
    // Associativity 16
    temp = set_associative(infile_name, 16*one_kb, 32, 16);
    outfile << temp.cache_hits << " " << temp.accesses << "; " << endl;

    // Fully Associative with LRU (0)
    temp = fully_associative(infile_name, 16*one_kb, 32, 0);
    outfile << temp.cache_hits << " " << temp.accesses << "; ";

    // Fully Associative with hot-cold (1)
    temp = fully_associative(infile_name, 16*one_kb, 32, 1);
    outfile << temp.cache_hits << " " << temp.accesses << "; ";

    return 0;
}


result direct_mapped(string infile_name, int cache_size, int cache_line_size){
	int cache_entries = cache_size / cache_line_size;
	vector<vector<unsigned long long>>cache (cache_entries, vector<unsigned long long> (cache_line_size, -1)); 

    ifstream infile = ifstream(infile_name);

    string line, behavior;
    unsigned long long address, base, cache_index, cache_line_index;
    result res;

    if (infile.is_open()){
        while(getline(infile, line)){
            res.accesses++;
            stringstream s(line);     
            s >> behavior >> hex >>  address;

            base = address / cache_line_size;
            cache_index = base % cache_entries;
            cache_line_index = address % cache_line_size;

            // base case (cache miss)
            if(cache[cache_index][cache_line_index] != address){
                for(int i = 0; i < cache_line_size; i++){
                    cache[cache_index][i] = base*cache_line_size + i;
                }
            } else {
                res.cache_hits++;
            }
            
        }
        infile.close();
    } 

    return res;
}

result set_associative(string infile_name, int cache_size, int cache_line_size, int associativity){
    ifstream infile = ifstream(infile_name);
    string line, behavior;
    result res;
    unsigned long long address, base, cache_index, cache_line_index;

    int cache_entries = cache_size / cache_line_size / associativity;
    cache_info temp;
    temp.last_used = 0;
    temp.cache_line.resize(cache_line_size, -1);

    // vector<dataType> name(size, value); 
    vector<vector<cache_info>> cache (associativity, vector<cache_info> (cache_entries, temp));


    if (infile.is_open()){
        while(getline(infile, line)){
            res.accesses++;
            stringstream s(line);     
            s >> behavior >> hex >> address;

            base = address / cache_line_size;
            cache_index = base % cache_entries;
            cache_line_index = address % cache_line_size;
            int hit = 0;
            

            for (int i = 0; i < cache.size(); i++){
                // cout << cache[i][cache_index].cache_line[cache_line_index] << endl;
                if (cache[i][cache_index].cache_line[cache_line_index] ==  address){
                    res.cache_hits++;
                    // cout << res.cache_hits << endl;
                    hit = 1;
                    cache[i][cache_index].last_used = res.accesses;
                }
            }

            if (hit == 0){
                unsigned long long min = 1000000000000;
                int victim = -1;

                for (int i = 0; i < cache.size(); i++){
                    if (cache[i][cache_index].last_used < min){
                        min = cache[i][cache_index].last_used;
                        victim = i;
                    }
                }

                cache[victim][cache_index].last_used = res.accesses;
                for (int i = 0; i < cache[victim][cache_index].cache_line.size(); i++){
                    cache[victim][cache_index].cache_line[i] = base*cache_line_size + i;
                }
            }
        }
        infile.close();
    } 

    return res;
}


result fully_associative(string infile_name, int cache_size, int cache_line_size, int replacement_policy){
    result res;

    // LRU
    if (replacement_policy == 0){
        res = set_associative(infile_name, cache_size, cache_line_size, cache_size/cache_line_size);
    }
    // Hot-Cold
    else if (replacement_policy == 1){
        ifstream infile = ifstream(infile_name);
        string line, behavior;
        result res;
        unsigned long long address, base, cache_index, cache_line_index;

        int cache_entries = cache_size / cache_line_size;
        vector<int> hot_cold_bits (cache_entries - 1, 0);
        cache_info temp;
        temp.last_used = 0;
        temp.cache_line.resize(cache_line_size, -1);

        while(getline(infile, line)){
            res.accesses++;
            stringstream s(line);     
            s >> behavior >> hex >> address;

            base = address / cache_line_size;
            cache_index = base % cache_entries;
            cache_line_index = address % cache_line_size;
            int hit = 0;
    }
    

    return res;
}