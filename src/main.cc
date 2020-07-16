#include <global.h>
#include <checkArgs.hpp>
//Rodrigo montenegro
uint64_t* g_numbers = nullptr;
uint64_t* ArreglodenumerosParalelo = nullptr;
uint64_t SumaenParalelo = 0;
std::vector<std::thread *> hilos;
std::vector<std::thread *> hilosSumas;
std::mutex Candado;
void LLenadoArreglo(size_t Left, size_t Right, size_t RNEtype){

	for(size_t i = Left; i < Right; ++i){   			
		switch(RNEtype){
			case 1:
				ArreglodenumerosParalelo[i] = g_numbers[i];
				break;
		}
	}
}
void SumadoenParalelo(uint32_t Left, uint32_t Right){
	Candado.lock();
	for(uint32_t i = Left; i < Right; i++){
		SumaenParalelo += ArreglodenumerosParalelo[i];
	}
	Candado.unlock();
}
int main(int argc, char** argv){

	uint64_t NumerostotaldeElementos;
	uint32_t numerosdeHilos;
	uint32_t Limiteinferior;
	uint32_t Limitesuperior;

	auto argumentos = (std::shared_ptr<checkArgs>) new checkArgs(argc, argv);
	
	NumerostotaldeElementos = argumentos->getArgs().tamanodeproblemas;
	numerosdeHilos     = argumentos->getArgs().numerosdeHilos;
	Limiteinferior	   = argumentos->getArgs().Limiteinferior;
	Limitesuperior 	   = argumentos->getArgs().Limitesuperior;

    std::cout << "Parametros N:t:l:L \n" << std::endl;
	std::cout << "Numero de elementos: " << NumerostotaldeElementos << std::endl;
	std::cout << "Numero de Hilos : " << numerosdeHilos  << std::endl;
	std::cout << "Numero random de Limite Inferior : "<< Limiteinferior << std::endl;
	std::cout << "Numero random de Limite Inferior : "<< Limitesuperior << std::endl;
   
	std::uniform_int_distribution<> unif(Limiteinferior, Limitesuperior);
	g_numbers = new uint64_t[NumerostotaldeElementos];
	std::srand(std::time(0)); 
	std::random_device device;
	std::mt19937 rng(device());

	for(uint32_t i = 0; i < NumerostotaldeElementos; i++){
		g_numbers[i] = unif(rng);
		
	}
    
	auto start = std::chrono::high_resolution_clock::now();
	auto end     = std::chrono::high_resolution_clock::now(); 
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	//Modulo LLenado en paralelo

	//llenado En Paralelo
	ArreglodenumerosParalelo = new uint64_t[NumerostotaldeElementos];
	for (size_t i= 0; i < numerosdeHilos ; i++){
		hilos.push_back(new std::thread(LLenadoArreglo, i*(NumerostotaldeElementos)/numerosdeHilos, (i+1)*(NumerostotaldeElementos)/numerosdeHilos,1));
	}
	start = std::chrono::high_resolution_clock::now();
	for(auto &thLlenado : hilos){
		thLlenado->join();
	}
	end     = std::chrono::high_resolution_clock::now(); 
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto totalTimeP = elapsed.count();
	//Modulo de sumado en paralelo
	//sumado de paralelo
	start     = std::chrono::high_resolution_clock::now(); 
	end     = std::chrono::high_resolution_clock::now(); 
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	for (size_t i=0;i<numerosdeHilos;i++){
		hilosSumas.push_back(new std::thread(SumadoenParalelo, i*(NumerostotaldeElementos)/numerosdeHilos,(i+1)*(NumerostotaldeElementos)/numerosdeHilos));
	}
	start     = std::chrono::high_resolution_clock::now();
	for(auto &thS : hilosSumas){
		thS->join();
	}
	end     = std::chrono::high_resolution_clock::now(); 
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto totalTimeSumaP = elapsed.count();
    std::cout << "stats "  << "\n" << std::endl;
	std::cout << "Suma Total en tiempos Paralelo: " << SumaenParalelo << std::endl;
	std::cout << "\n" <<  "Tiempos de Llenado"  << "\n" << std::endl;
	std::cout << "Tiempo llenado en Paralelo:"  << totalTimeP << "[ms]" << std::endl;
	std::cout << "\n" <<  "Tiempos de Sumado" << "\n" << std::endl;
	std::cout << "Tiempo sumado en Paralelo:"  << totalTimeSumaP << "[ms]" << std::endl;
    std::cout << "tiempo total en paralelo,'llenado'+'suma': " << totalTimeSumaP + totalTimeP << "[ms]" << std::endl;
    std::cout << "tiempo total : " << totalTimeP /totalTimeSumaP  << "[ms]" << std::endl;
   


	return(EXIT_SUCCESS);
}

