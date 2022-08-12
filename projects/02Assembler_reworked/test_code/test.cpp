#include<iostream> 
#include<fstream>

std::ostream& operator << (std::ostream& os, const std::fstream& file_handle)
    {
      os<< file_handle; 
      return os;
    }

class car {
	private:
	int rating;
	
	public:
	std::string brand;
	std::string name;

    void exception() {
        try
        {
            std::cin >> brand;
            throw 505;
        }
        catch(int error)
        {
            std::cout << "Error Message";
        }
        
    }
};

   
int main()
    {
        std::fstream file_handle;
        file_handle.open("test.txt", std::ios::app); 
        car vehicle1;  //created an object, calls constructor by default
        vehicle1.exception();
        return 0;
    }

