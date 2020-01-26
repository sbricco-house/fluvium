#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "modem_dce.h"

/**
 * @brief Indicate that processing current command has done
 *
 * @param dce Modem DCE object
 * @param state Modem state after processing
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static inline esp_err_t esp_modem_process_command_done(modem_dce_t *dce, modem_state_t state)
{
    dce->state = state;
    return dce->dte->process_cmd_done(dce->dte);
}

/**
 * @brief Strip the tailed "\r\n"
 *
 * @param str string to strip
 * @param len length of string
 */
static inline void strip_cr_lf_tail(char *str, uint32_t len)
{
    if (str[len - 2] == '\r') {
        str[len - 2] = '\0';
    } else if (str[len - 1] == '\r') {
        str[len - 1] = '\0';
    }
}

/**
 * @brief Default handler for response
 * Some responses for command are simple, commonly will return OK when succeed of ERROR when failed
 *
 * @param dce Modem DCE object
 * @param line line string
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_handle_response_default(modem_dce_t *dce, const char *line);

/**
 * @brief Syncronization
 *
 * @param dce Modem DCE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_sync(modem_dce_t *dce);

/**
 * @brief Store current parameter setting in the user profile
 *
 * @param dce Modem DCE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_store_profile(modem_dce_t *dce);

/**
 * @brief Set flow control mode of DCE in data mode
 *
 * @param dce Modem DCE object
 * @param flow_ctrl flow control mode
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_set_flow_ctrl(modem_dce_t *dce, modem_flow_ctrl_t flow_ctrl);

/**
 * @brief Define PDP context
 *
 * @param dce Modem DCE object
 * @param cid PDP context identifier
 * @param type Protocol type
 * @param apn Access point name
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_define_pdp_context(modem_dce_t *dce, uint32_t cid, const char *type, const char *apn);

/**
 * @brief Hang up
 *
 * @param dce Modem DCE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t esp_modem_dce_hang_up(modem_dce_t *dce);

#ifdef __cplusplus
}
#endif