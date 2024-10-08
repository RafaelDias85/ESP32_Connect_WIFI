#include "wifi.h"

#define TAG "CONNECT"

// define o tempo de conexao
#define CONNECT_TIME_OUT 5000u

#define CONNECTED_GOT_IP BIT0

EventGroupHandle_t wifi_event;

static volatile uint8_t _IsOnLine = 0;

esp_netif_ip_info_t ip_info; // Estrutura para armazenar informa��es do endere�o IP, incluindo IP, m�scara de sub-rede e gateway.
esp_netif_t *netif_handle;   // Declara��o do ponteiro para a interface de rede

char my_ip[16];

// fun��o para tratar os erros de wifi conforme os codigos de erro:
static char *get_wifi_err(uint8_t Errcode)
{
    // Verifica o codigo de erro e retorna a string correspondente
    switch (Errcode)
    {
    case WIFI_REASON_UNSPECIFIED:
        return "WIFI_REASON_UNSPECIFIED"; // Causa n�o especificada.
    case WIFI_REASON_AUTH_EXPIRE:
        return "WIFI_REASON_AUTH_EXPIRE"; // A autentica��o expirou.
    case WIFI_REASON_AUTH_LEAVE:
        return "WIFI_REASON_AUTH_LEAVE"; // O dispositivo deixou a autentica��o.
    case WIFI_REASON_ASSOC_EXPIRE:
        return "WIFI_REASON_ASSOC_EXPIRE"; // A associa��o expirou.
    case WIFI_REASON_ASSOC_TOOMANY:
        return "WIFI_REASON_ASSOC_TOOMANY"; // O dispositivo tentou se associar a muitos pontos de acesso.
    case WIFI_REASON_NOT_AUTHED:
        return "WIFI_REASON_NOT_AUTHED"; // N�o autenticado.
    case WIFI_REASON_NOT_ASSOCED:
        return "WIFI_REASON_NOT_ASSOCED"; // N�o associado a um ponto de acesso.
    case WIFI_REASON_ASSOC_LEAVE:
        return "WIFI_REASON_ASSOC_LEAVE"; // O dispositivo saiu da associa��o.
    case WIFI_REASON_ASSOC_NOT_AUTHED:
        return "WIFI_REASON_ASSOC_NOT_AUTHED"; // N�o autenticado durante a associa��o.
    case WIFI_REASON_DISASSOC_PWRCAP_BAD:
        return "WIFI_REASON_DISASSOC_PWRCAP_BAD"; // Desassocia��o devido a capacidade de pot�ncia ruim.
    case WIFI_REASON_DISASSOC_SUPCHAN_BAD:
        return "WIFI_REASON_DISASSOC_SUPCHAN_BAD"; // Desassocia��o devido a canais suportados ruins.
    case WIFI_REASON_BSS_TRANSITION_DISASSOC:
        return "WIFI_REASON_BSS_TRANSITION_DISASSOC"; // Desassocia��o devido a transi��o de BSS (Basic Service Set).
    case WIFI_REASON_IE_INVALID:
        return "WIFI_REASON_IE_INVALID"; // Elemento de informa��o inv�lido.
    case WIFI_REASON_MIC_FAILURE:
        return "WIFI_REASON_MIC_FAILURE"; // Falha no c�digo de integridade da mensagem.
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
        return "WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT"; // Timeout no handshake de 4 vias.
    case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT:
        return "WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT"; // Timeout na atualiza��o da chave do grupo.
    case WIFI_REASON_IE_IN_4WAY_DIFFERS:
        return "WIFI_REASON_IE_IN_4WAY_DIFFERS"; // Elemento de informa��o em 4 vias � diferente.
    case WIFI_REASON_GROUP_CIPHER_INVALID:
        return "WIFI_REASON_GROUP_CIPHER_INVALID"; // Cifra de grupo inv�lida.
    case WIFI_REASON_PAIRWISE_CIPHER_INVALID:
        return "WIFI_REASON_PAIRWISE_CIPHER_INVALID"; // Cifra par a par inv�lida.
    case WIFI_REASON_AKMP_INVALID:
        return "WIFI_REASON_AKMP_INVALID"; // AKMP (Authentication and Key Management Protocol) inv�lido.
    case WIFI_REASON_UNSUPP_RSN_IE_VERSION:
        return "WIFI_REASON_UNSUPP_RSN_IE_VERSION"; // Vers�o IE do RSN (Robust Security Network) n�o suportada.
    case WIFI_REASON_INVALID_RSN_IE_CAP:
        return "WIFI_REASON_INVALID_RSN_IE_CAP"; // Capacidade IE do RSN inv�lida.
    case WIFI_REASON_802_1X_AUTH_FAILED:
        return "WIFI_REASON_802_1X_AUTH_FAILED"; // Falha na autentica��o 802.1X.
    case WIFI_REASON_CIPHER_SUITE_REJECTED:
        return "WIFI_REASON_CIPHER_SUITE_REJECTED"; // Conjunto de cifras rejeitado.
    case WIFI_REASON_INVALID_PMKID:
        return "WIFI_REASON_INVALID_PMKID"; // PMKID (Pairwise Master Key Identifier) inv�lido.
    case WIFI_REASON_BEACON_TIMEOUT:
        return "WIFI_REASON_BEACON_TIMEOUT"; // Timeout no beacon (sinalizador).
    case WIFI_REASON_NO_AP_FOUND:
        return "WIFI_REASON_NO_AP_FOUND"; // Nenhum ponto de acesso encontrado.
    case WIFI_REASON_AUTH_FAIL:
        return "WIFI_REASON_AUTH_FAIL"; // Falha na autentica��o.
    case WIFI_REASON_ASSOC_FAIL:
        return "WIFI_REASON_ASSOC_FAIL"; // Falha na associa��o.
    case WIFI_REASON_HANDSHAKE_TIMEOUT:
        return "WIFI_REASON_HANDSHAKE_TIMEOUT"; // Timeout no handshake.
    case WIFI_REASON_CONNECTION_FAIL:
        return "WIFI_REASON_CONNECTION_FAIL"; // Falha na conex�o.
    case WIFI_REASON_AP_TSF_RESET:
        return "WIFI_REASON_AP_TSF_RESET"; // Reset do TSF (Timestamp Field) do ponto de acesso.
    case WIFI_REASON_ROAMING:
        return "WIFI_REASON_ROAMING"; // O dispositivo est� em roaming.
    }

    // Se o erro nao foi encontrado, retorna "WIFI_REASON_UNSPECIFIED"
    return "WIFI_REASON_UNSPECIFIED"; // MOTIVO DO WIFI N�O ESPECIFICADO.
}

static void wifi_event_handle(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch ( event_id )
    {
        case WIFI_EVENT_STA_START: // A interface STA foi iniciada, tente se conectar a uma rede Wi-Fi

            esp_wifi_connect();
            ESP_LOGI(TAG, "Wifi start");
            _IsOnLine = 0; // Define que o dispositivo ainda n�o est� online
            break;

    }

}