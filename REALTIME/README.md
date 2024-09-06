# Firebase Realtime Database Client

Este projeto contém um cliente para o Firebase Realtime Database utilizando o ESP-IDF. Ele inclui funções para conectar ao Firebase, ler e escrever dados em tempo real, e gerenciar eventos HTTPs.

O client mantém uma conexão contínua com o Firebase Realtime Database através de 'text/event-stream', garantindo atualizações em tempo real.

# Funções Principais

• Conexão HTTPs: Conecta ao Firebase Realtime Database usando HTTPS.
• Leitura de Dados: Lê dados do Firebase em tempo real.
• Envio de Dados: Envia dados para o Firebase Realtime Database.