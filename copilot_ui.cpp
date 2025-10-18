// copilot_ui.cpp - A Interface do Copilot (UI em C++)

#include <iostream>
#include <string>
#include <cstdio>
#include <stdexcept>
#include <thread>
#include <sstream>

// Definindo o comando para executar o engine Python.
// Certifique-se de que 'python3' ou 'python' está no seu PATH.
#ifdef _WIN32
    // Use '_popen' no Windows
    const std::string PYTHON_CMD = "python ai_engine.py";
#else
    // Use 'popen' em sistemas Unix/Linux/macOS
    const std::string PYTHON_CMD = "python3 ai_engine.py"; 
#endif

/**
 * @brief Envia um comando para o processo Python e lê a resposta.
 * @param pipe Ponteiro para o FILE* do pipe aberto.
 * @param prompt A string do usuário a ser enviada.
 * @return A resposta do Copilot (texto puro) ou uma mensagem de erro.
 */
std::string send_and_receive(FILE* pipe, const std::string& prompt) {
    if (!pipe) {
        return "ERRO: O motor da IA (Python) não está ativo ou falhou.";
    }

    // 1. Envia o prompt para o stdin do Python
    std::string command = prompt + "\n";
    if (fputs(command.c_str(), pipe) == EOF) {
        return "ERRO: Falha ao enviar o prompt para a IA.";
    }
    // Garante que o prompt é enviado imediatamente
    fflush(pipe); 

    // 2. Lê a resposta do stdout do Python
    char buffer[4096];
    std::string result = "";
    
    // Ler a resposta do pipe até que o Python envie uma linha e um newline.
    if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result = buffer;
    } else {
        return "ERRO: Motor da IA encerrado inesperadamente (Verifique a chave API).";
    }

    // 3. Processa a resposta (Procura pelo código de status)
    if (result.rfind("[RESULTADO_OK]", 0) == 0) {
        return result.substr(sizeof("[RESULTADO_OK]") - 1); // Remove a tag de status
    } else if (result.rfind("[ERRO_API]", 0) == 0) {
        return "[ERRO DA API - Python Engine] " + result.substr(sizeof("[ERRO_API]") - 1);
    } else if (result.rfind("[ERRO_Geral]", 0) == 0) {
        return "[ERRO GERAL - Python Engine] " + result.substr(sizeof("[ERRO_Geral]") - 1);
    }

    return "Resposta da IA em formato inesperado:\n" + result;
}

void run_ui() {
    std::cout << "Iniciando o Copilot UI (C++) e o motor da IA (Python)..." << std::endl;
    
    // Inicia o processo Python e abre o pipe de comunicacao bidirecional
    FILE* ai_pipe = nullptr;
    #ifdef _WIN32
        ai_pipe = _popen(PYTHON_CMD.c_str(), "r+");
    #else
        ai_pipe = popen(PYTHON_CMD.c_str(), "r+");
    #endif

    if (!ai_pipe) {
        std::cerr << "ERRO: Falha ao iniciar o processo Python. Verifique o PATH e o nome do arquivo 'ai_engine.py'." << std::endl;
        return;
    }
    
    // Aguarda a confirmacao de pronto do Python
    char ready_buffer[64];
    if (fgets(ready_buffer, sizeof(ready_buffer), ai_pipe) == NULL || 
        std::string(ready_buffer).find("AI_ENGINE_READY") == std::string::npos) 
    {
        std::cerr << "ERRO: O motor Python nao enviou o sinal 'AI_ENGINE_READY' ou falhou na inicializacao. (Verifique 'stderr' do Python)." << std::endl;
        
        #ifdef _WIN32
            _pclose(ai_pipe);
        #else
            pclose(ai_pipe);
        #endif
        return;
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "      Copilot (UI C++ / AI Python)      " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "O motor da IA esta pronto. Digite 'sair' para fechar." << std::endl;

    // Loop de interacao da UI (simulacao de entrada de texto)
    std::string user_input;
    while (true) {
        std::cout << "\n>>> Seu Prompt: ";
        std::getline(std::cin, user_input);

        if (user_input.empty()) continue;

        if (user_input == "sair") {
            // Envia o comando 'sair' para o Python e fecha o processo
            fputs("sair\n", ai_pipe);
            fflush(ai_pipe);
            break; 
        }

        // Simula o envio do prompt e a exibicao da resposta
        std::string copilot_response = send_and_receive(ai_pipe, user_input);
        
        std::cout << "\n--- Resposta do Copilot ---\n";
        std::cout << copilot_response << std::endl;
        std::cout << "---------------------------\n";
    }

    // Fecha o pipe e aguarda o processo Python terminar
    std::cout << "Encerrando motor da IA..." << std::endl;
    #ifdef _WIN32
        _pclose(ai_pipe);
    #else
        pclose(ai_pipe);
    #endif
    std::cout << "Programa finalizado." << std::endl;
}

int main() {
    run_ui();
    return 0;
}
