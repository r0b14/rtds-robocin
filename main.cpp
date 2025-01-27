#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

// estrutura que representa uma linha de dados do CSV
struct SensorData {
    float local_w;         // Velocidade angular (em rad/s)
    float odometry_theta;  // Ângulo "medido" (em rad)
};

/**
 * Preciso fazer com que os dados lido do arquvio sejam convertidos e inseridos em um vector.
 * Função para ler o arquivo CSV e retornar um vetor de SensorData
 */
std::vector<SensorData> readCsv(const std::string& filename) {
    std::vector<SensorData> data;
    std::ifstream file(filename);

    // Verificando se o arquivo foi aberto corretamente e retornando um vetor vazio
    if (!file.is_open()) {
        std::cerr << "ERRO!: " << filename << std::endl;
        return data;  //vetor vazio
    }

    std::string line;
    bool isHeader = true;

    while (std::getline(file, line)) {
        // Se a primeira linha for cabeçalho, apenas ignorar
        if (isHeader) {
            isHeader = false;
            continue;
        }

        // Quebrando a linha em duas partes (local_w e odometry_theta)
        std::stringstream ss(line);
        std::string col1, col2;
        if (std::getline(ss, col1, ',') && std::getline(ss, col2, ',')) {
            try {
                SensorData sd;
                sd.local_w = std::stof(col1);
                sd.odometry_theta = std::stof(col2);
                data.push_back(sd);
            } catch (const std::exception &e) {
                std::cerr << "Erro de conversao de dados na linha: " << line
                          << " - " << e.what() << std::endl;
            }
        }
    }

    file.close();
    return data;
}

int main() {
    const std::string csvFile = "dados.csv"; //nome do arquivo CSV

    // Lendo dados do arquivo
    std::vector<SensorData> dataVector = readCsv(csvFile);

    // Caso não tenha nada, nem roda. 
    if (dataVector.empty()) {
        std::cerr << "Nenhum dado foi lido do arquivo." << std::endl;
        return 1;
    }
    /**
     * Integração de local_w (velocidade angular) ao longo do tempo
     * para estimar um ângulo (angle_calc).
     * Supondo um intervalo de amostragem fixo, dt.
     */
    float dt = 0.1f;  // 0.1s = 10 Hz de amostragem (exemplo)
    float angle_calc = 0.0f;

    std::cout << "Processando " << dataVector.size() 
              << " linhas de dados:\n\n";

    for (size_t i = 0; i < dataVector.size(); i++) {
        // Integrar a velocidade angular
        angle_calc += dataVector[i].local_w * dt; //angle_calc(n+1) = angle_calc(n)+(local_wn*Δt).

        /**
            float local_w_rad = dataVector[i].local_w * (3.14159f / 180.0f);
            angle_calc += local_w_rad * dt;
         */

        // Exibir resultados da "linha i"
        std::cout << "Linha " << i
                  << " | local_w = " << dataVector[i].local_w
                  << " rad/s, odometry_theta = " << dataVector[i].odometry_theta
                  << " rad, angle_calc (integrado) = " << angle_calc << " rad\n";
    }

    // Ao final, podemos comparar "angle_calc" com o último odometry_theta
    std::cout << "\nValor final de angle_calc: " << angle_calc << " rad\n";
    std::cout << "Valor final de odometry_theta (ultimo registro): "
              << dataVector.back().odometry_theta << " rad\n";

    return 0;
}
