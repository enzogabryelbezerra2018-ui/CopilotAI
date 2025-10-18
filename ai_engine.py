# ai_engine.py - O Cérebro do Copilot (Lógica em Python)

import os
import sys
import json
from google import genai
from google.genai.errors import APIError

def initialize_gemini():
    """Inicializa o cliente Gemini e o objeto Chat."""
    # O token DEVE ser definido como uma variável de ambiente: GEMINI_API_KEY
    api_key = os.getenv("AIzaSyAWae7tn-Bgt4mbGcJK2jWFsbCYU2nkj-g")
    if not api_key:
        # Se a chave não for encontrada, retornamos um objeto mock para evitar crash
        print("ERRO_CHAVE: Variável GEMINI_API_KEY não definida.", file=sys.stderr)
        return None, None 

    try:
        client = genai.Client(api_key=api_key)
        
        system_instruction = (
            "Você é o Copilot, um assistente de IA especializado em programação C/C++, "
            "engenharia de software e formatos de arquivo. Responda a todas as perguntas de forma concisa e útil."
        )
        
        chat = client.chats.create(
            model="gemini-2.5-flash",
            config={"system_instruction": system_instruction}
        )
        
        return client, chat
    except Exception as e:
        print(f"ERRO_INIT: Falha ao inicializar o modelo. Detalhes: {e}", file=sys.stderr)
        return None, None

def run_engine(chat):
    """Loop principal para processar prompts vindos do stdin."""
    # Sinaliza que o motor está pronto para receber comandos
    print("AI_ENGINE_READY") 
    sys.stdout.flush() 

    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        
        if line.lower() == "sair":
            break

        try:
            # 1. Envia o prompt para o modelo
            response = chat.send_message(line)
            
            # 2. Retorna a resposta para o C++ no formato de dados simples (string)
            # Nota: Poderíamos usar JSON para dados mais complexos.
            
            # Formato de saída para C++: [RESULTADO_OK] + TEXTO_DA_RESPOSTA
            print(f"[RESULTADO_OK]{response.text}")
            
        except APIError as e:
            # Formato de saída para C++: [ERRO_API] + MENSAGEM_DE_ERRO
            print(f"[ERRO_API]Falha na comunicacao com Gemini: {e}")
        except Exception as e:
            print(f"[ERRO_Geral]Erro inesperado: {e}")

        # Garante que a resposta seja enviada imediatamente para o C++
        sys.stdout.flush()

if __name__ == "__main__":
    client, chat = initialize_gemini()
    
    if chat:
        run_engine(chat)
    else:
        # Se falhou na inicialização (ex: chave inválida), apenas espera o encerramento.
        # A mensagem de erro já foi enviada para stderr.
        pass

