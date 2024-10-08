#include "wifi.h"

#define TAG "CONNECT"

// define o tempo de conexao
#define CONNECT_TIME_OUT 5000u

#define CONNECTED_GOT_IP BIT0

EventGroupHandle_t wifi_event;

static volatile uint8_t _IsOnLine = 0;

esp_netif_ip_info_t ip_info; // Estrutura para armazenar informações do endereço IP, incluindo IP, máscara de sub-rede e gateway.
esp_netif_t *netif_handle;   // Declaração do ponteiro para a interface de rede

char my_ip[16];

// função para tratar os erros de wifi conforme os codigos de erro:
static char *get_wifi_err(uint8_t Errcode)
{
    // Verifica o codigo de erro e retorna a string correspondente
    switch (Errcode)
    {
    case WIFI_REASON_UNSPECIFIED:
        return "WIFI_REASON_UNSPECIFIED"; // Causa não especificada.
    case WIFI_REASON_AUTH_EXPIRE:
        return "WIFI_REASON_AUTH_EXPIRE"; // A autenticação expirou.
    case WIFI_REASON_AUTH_LEAVE:
        return "WIFI_REASON_AUTH_LEAVE"; // O dispositivo deixou a autenticação.
    case WIFI_REASON_ASSOC_EXPIRE:
        return "WIFI_REASON_ASSOC_EXPIRE"; // A associação expirou.
    case WIFI_REASON_ASSOC_TOOMANY:
        return "WIFI_REASON_ASSOC_TOOMANY"; // O dispositivo tentou se associar a muitos pontos de acesso.
    case WIFI_REASON_NOT_AUTHED:
        return "WIFI_REASON_NOT_AUTHED"; // Não autenticado.
    case WIFI_REASON_NOT_ASSOCED:
        return "WIFI_REASON_NOT_ASSOCED"; // Não associado a um ponto de acesso.
    case WIFI_REASON_ASSOC_LEAVE:
        return "WIFI_REASON_ASSOC_LEAVE"; // O dispositivo saiu da associação.
    case WIFI_REASON_ASSOC_NOT_AUTHED:
        return "WIFI_REASON_ASSOC_NOT_AUTHED"; // Não autenticado durante a associação.
    case WIFI_REASON_DISASSOC_PWRCAP_BAD:
        return "WIFI_REASON_DISASSOC_PWRCAP_BAD"; // Desassociação devido a capacidade de potência ruim.
    case WIFI_REASON_DISASSOC_SUPCHAN_BAD:
        return "WIFI_REASON_DISASSOC_SUPCHAN_BAD"; // Desassociação devido a canais suportados ruins.
    case WIFI_REASON_BSS_TRANSITION_DISASSOC:
        return "WIFI_REASON_BSS_TRANSITION_DISASSOC"; // Desassociação devido a transição de BSS (Basic Service Set).
    case WIFI_REASON_IE_INVALID:
        return "WIFI_REASON_IE_INVALID"; // Elemento de informação inválido.
    case WIFI_REASON_MIC_FAILURE:
        return "WIFI_REASON_MIC_FAILURE"; // Falha no código de integridade da mensagem.
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
        return "WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT"; // Timeout no handshake de 4 vias.
    case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT:
        return "WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT"; // Timeout na atualização da chave do grupo.
    case WIFI_REASON_IE_IN_4WAY_DIFFERS:
        return "WIFI_REASON_IE_IN_4WAY_DIFFERS"; // Elemento de informação em 4 vias é diferente.
    case WIFI_REASON_GROUP_CIPHER_INVALID:
        return "WIFI_REASON_GROUP_CIPHER_INVALID"; // Cifra de grupo inválida.
    case WIFI_REASON_PAIRWISE_CIPHER_INVALID:
        return "WIFI_REASON_PAIRWISE_CIPHER_INVALID"; // Cifra par a par inválida.
    case WIFI_REASON_AKMP_INVALID:
        return "WIFI_REASON_AKMP_INVALID"; // AKMP (Authentication and Key Management Protocol) inválido.
    case WIFI_REASON_UNSUPP_RSN_IE_VERSION:
        return "WIFI_REASON_UNSUPP_RSN_IE_VERSION"; // Versão IE do RSN (Robust Security Network) não suportada.
    case WIFI_REASON_INVALID_RSN_IE_CAP:
        return "WIFI_REASON_INVALID_RSN_IE_CAP"; // Capacidade IE do RSN inválida.
    case WIFI_REASON_802_1X_AUTH_FAILED:
        return "WIFI_REASON_802_1X_AUTH_FAILED"; // Falha na autenticação 802.1X.
    case WIFI_REASON_CIPHER_SUITE_REJECTED:
        return "WIFI_REASON_CIPHER_SUITE_REJECTED"; // Conjunto de cifras rejeitado.
    case WIFI_REASON_INVALID_PMKID:
        return "WIFI_REASON_INVALID_PMKID"; // PMKID (Pairwise Master Key Identifier) inválido.
    case WIFI_REASON_BEACON_TIMEOUT:
        return "WIFI_REASON_BEACON_TIMEOUT"; // Timeout no beacon (sinalizador).
    case WIFI_REASON_NO_AP_FOUND:
        return "WIFI_REASON_NO_AP_FOUND"; // Nenhum ponto de acesso encontrado.
    case WIFI_REASON_AUTH_FAIL:
        return "WIFI_REASON_AUTH_FAIL"; // Falha na autenticação.
    case WIFI_REASON_ASSOC_FAIL:
        return "WIFI_REASON_ASSOC_FAIL"; // Falha na associação.
    case WIFI_REASON_HANDSHAKE_TIMEOUT:
        return "WIFI_REASON_HANDSHAKE_TIMEOUT"; // Timeout no handshake.
    case WIFI_REASON_CONNECTION_FAIL:
        return "WIFI_REASON_CONNECTION_FAIL"; // Falha na conexão.
    case WIFI_REASON_AP_TSF_RESET:
        return "WIFI_REASON_AP_TSF_RESET"; // Reset do TSF (Timestamp Field) do ponto de acesso.
    case WIFI_REASON_ROAMING:
        return "WIFI_REASON_ROAMING"; // O dispositivo está em roaming.
    }

    // Se o erro nao foi encontrado, retorna "WIFI_REASON_UNSPECIFIED"
    return "WIFI_REASON_UNSPECIFIED"; // MOTIVO DO WIFI NÃO ESPECIFICADO.
}

static void wifi_event_handle(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch ( event_id )
    {
        case WIFI_EVENT_STA_START: // A interface STA foi iniciada, tente se conectar a uma rede Wi-Fi

            esp_wifi_connect();
            ESP_LOGI(TAG, "Wifi start");
            _IsOnLine = 0; // Define que o dispositivo ainda não está online
            break;

    }

}