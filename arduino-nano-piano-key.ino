#include <Arduino.h>
#include "SequenceTracker.hpp"

// Номер цифрового пятивольтового вывода, конфигурируемого
// как выход, на который подаётся логическая единица, когда
// последовательность нажатий на клавиши пианино совпадает
// с ожидаемой. 
constexpr int STATUS_PIN_MELODY_MATCH = 10;

// Номер цифрового пятивольтового вывода, конфигурируемого
// как выход, на который подаётся логическая единица всё
// время, пока последовательность нажатий на клавиши пианино
// не совпадает с ожидаемой.
constexpr int STATUS_PIN_MELODY_NOT_MATCH = 11;

// Количество выводов, задействованных под клавиши пианино.
constexpr int PIANO_PIN_COUNT = 7;

// Назначение цифровых пятивольтовых выводов клавишам пианино.
const int PIANO_PINS[PIANO_PIN_COUNT] = {1, 2, 3, 4, 5, 6, 7};

// Массив для хранения предыдущего состояния клавиш пианино
// с целью сравнения со считываемым состоянием соответствующих
// выводов.
int piano_pin_state[PIANO_PIN_COUNT];

// Количество нот в мелодии.
constexpr int MELODY_LENGTH = 4;

// Мелодия. Клавиши пианино нумеруются с нуля (фактически,
// индекс в PIANO_PINS).
const int melody[MELODY_LENGTH] = {0, 3, 1, 2};

// Объект для отслеживания мелодии.
using MelodyTracker = SequenceTracker<int>;
MelodyTracker melody_tracker;

/** Конфигурирует выводы устройства и настраивает
 * отслеживание мелодии. */
void setup() {
    // Конфигурируем выводы, подключенные к клавишам пианино.
    setup_piano_pins();

    // Конфигурируем выводы-индикаторы статуса.
    setup_status_pins();

    // Настраиваем объект для отслеживания мелодии.
    setup_melody_tracker();
}

void loop() {
    // Проверяем состояние выводов, подключенных к клавишам пианино.
    // При изменении состояния на "нажата" передаем номер нажатой
    // клавиши в объект, отслеживающий мелодию. 
    for (int i = 0; i < PIANO_PIN_COUNT; ++i) {
        const int state = digitalRead(PIANO_PINS[i]);
        if ((state == LOW) && (state != piano_pin_state[i])) {
            melody_tracker.process(i);
        }
        piano_pin_state[i] = state;
    }
}

/** Конфигурирует выводы, подключенные к клавишам пианино, как
 * входы. Активирует встроенные нагрузочные резисторы, которые
 * притягивают эти выводы к питанию. Предполагается, что
 * нажатия на клавиши пианино замыкает эти выводы на землю.
 */
void setup_piano_pins() {
    for (int i = 0; i < PIANO_PIN_COUNT; ++i) {
        pinMode(PIANO_PINS[i], INPUT);
        digitalWrite(PIANO_PINS[i], HIGH);
        piano_pin_state[i] = HIGH;
    }
}

/** Конфигурирует выводы, предназначенные для индикации статуса,
 * как выходы и мигает ими. */
void setup_status_pins() {
    pinMode(STATUS_PIN_MELODY_MATCH, OUTPUT);
    pinMode(STATUS_PIN_MELODY_NOT_MATCH, OUTPUT);

    digitalWrite(STATUS_PIN_MELODY_MATCH, HIGH);
    digitalWrite(STATUS_PIN_MELODY_NOT_MATCH, HIGH);
    
    delay(300);

    digitalWrite(STATUS_PIN_MELODY_MATCH, LOW);
    digitalWrite(STATUS_PIN_MELODY_NOT_MATCH, LOW);

    delay(300);
}

/** Настраивает объект для отслеживания совпадения мелодии
 * с ожидаемой. */
void setup_melody_tracker() {
    melody_tracker.set_sequence(&melody[0], MELODY_LENGTH);
    melody_tracker.set_listener(&on_melody_tracker_event);
    // Обновляем статус.
    melody_tracker.notify();
}

/** Действия, выполняемые при изменении состояния: совпадает
 * или нет последовательность нажатий клавиш с заданой мелодией.
 * Обновляет состояние выводов, отображающих статус.
 */
void on_melody_tracker_event(const MelodyTracker& sender) {
    bool melody_match = sender.get_state();
    digitalWrite(STATUS_PIN_MELODY_MATCH, melody_match? HIGH : LOW);
    digitalWrite(STATUS_PIN_MELODY_NOT_MATCH, melody_match? LOW : HIGH); 
}

