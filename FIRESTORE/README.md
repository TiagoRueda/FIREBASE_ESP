# ESP32 Firestore Integration

Este projeto demonstra como usar o ESP32 para interagir com a API do Firestore do Firebase usando HTTPs. O código fornece exemplos de como realizar operações básicas no Firestore, incluindo criação, atualização, leitura e exclusão de documentos.

# Funções Principais

void send_firestore(const char *path, int method, const char *data_doc): Envia uma solicitação HTTPs para o Firestore.
void post_doc(const char *data): Cria um novo documento no Firestore.
void patch_data(const char *data, const char *mask): Atualiza um documento existente no Firestore (usar para atualizar um campo especifico).
void get_doc(): Obtém um documento do Firestore.
void delete_doc(): Exclui um documento do Firestore.