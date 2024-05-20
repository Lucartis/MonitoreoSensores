//
// Created by jose-jaramillo on 3/26/24.
//
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

bool is_float(const std::string& str) {
    try {
        std::size_t pos;
        std::stof(str, &pos);
        // La conversión tiene éxito si no hay caracteres restantes en la cadena
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

// Función para verificar si una cadena representa un número entero
bool is_integer(const std::string& str) {
    try {
        std::size_t pos;
        std::stoi(str, &pos);
        // La conversión tiene éxito si no hay caracteres restantes en la cadena
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

int main(int argc, char *argv[]) {
    // Iniciando variables
    int opt;
    int sensorType = 0;
    int timeInterval = 0;
    char* fileName = nullptr;
    char* pipeName = nullptr;
    //Revisando argumentos y asignandolos
    while ((opt = getopt(argc, argv, "s:t:f:p:")) != -1) {
        switch (opt) {
            case 's':
                sensorType = atoi(optarg);
                break;
            case 't':
                timeInterval = atoi(optarg);
                break;
            case 'f':
                fileName = optarg;
                break;
            case 'p':
                pipeName = optarg;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " -s sensorType -t timeInterval -f fileName -p pipeName" << std::endl;
                return 1;
        }
    }

    // Abriendo archivo de lectura
    std::ifstream dataFile(fileName);
    if (!dataFile.is_open()) {
        std::cerr << "Failed to open data file: " << fileName << std::endl;
        return 1;
    }

    // Abriendo pipe
    int pipeFd;
    do {
        pipeFd = open(pipeName, O_WRONLY | O_NONBLOCK);
        if (pipeFd < 0) {
            std::cerr << "Failed to open pipe: " << pipeName << ", retrying..." << std::endl;
            sleep(1);
        }
    } while (pipeFd < 0);

    // Leyendo archivo y escribiendo en pipe
    std::string line;
    while (std::getline(dataFile, line)) {
        // Verificar el tipo de sensor
        if (sensorType == 1) { // Temperatura
            if (is_integer(line)) {
                write(pipeFd, line.c_str(), line.size() + 1);
                std::cerr << line << std::endl;
                sleep(timeInterval);

            }
        } else if (sensorType == 2) { // PH
            if (!is_integer(line)) {
                if (is_float(line)){
                    write(pipeFd, line.c_str(), line.size() + 1);
                    std::cerr << line << std::endl;
                    sleep(timeInterval);
                }

            }
        } else {
            std::cerr << "Invalid sensor type: " << sensorType << std::endl;
        }
    }

    // Cerrand archivo y pipe
    dataFile.close();
    close(pipeFd);

    return 0;
}