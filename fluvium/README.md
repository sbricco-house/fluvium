### ESP32: Possible power save solution

- Differenti modalità di power save: https://lastminuteengineers.com/esp32-sleep-modes-power-consumption/

- Blog in cui vengono trattate varie problematiche e soluzioni riguardo il power management https://blog.voneicken.com/2018/lp-wifi-esp8266-1/.

- ESP32 supporta automaticamente la modalità light sleep

  - ```c++
    esp_pm_config_esp32_t powersave_config;
    powersave_config.light_sleep_enable = true;
    esp_pm_configure(&powersave_config);
    ```

  - Automaticamente quando non ci sono task, assenza di segnali provenienti da UART o GPIO cambia le frequenze della CPU ed entra in questa modalità. Associazione tra WiFi e Access Point viene mantenuta, risvegliando il chip ad ogni DTIM beacon (il tempo DTIM può essere variato). TODO: a livello trasporto (es. TCP) resta attiva?

  - Si può selezione anche frequenza minima e massima del chip.

    ```
    esp_pm_config_esp32_t powersave_config;
    powersave_config.max_freq_mhz = 160;
    powersave_config.min_freq_mhz = 80;
    ```

    

- Comunicazione con stazione secondaria: si può utilizzare **ESP-Now** per creare rete peer-to-peer, si dice essere già low-power, è un protocollo proprietario EspressIf. Con questo procotollo non si applicano i discorsi di associazione tra WiFi e Access Point.

- TODO: valutare consumo effettivo con Amperometro.







