#ifndef _SEQUENCE_TRACKER_HPP_
#define _SEQUENCE_TRACKER_HPP_

/** Шаблон класса, предназначенного для отслеживания определённой
 * последовательности значений в передаваемых ему входных данных. */
template <typename T, int buffer_size = 1024>
class SequenceTracker {
    public:
        /** Тип указателя на функцию обратной связи. */
        typedef void (*Listener) (
            const SequenceTracker<T, buffer_size>& sender
        );

    private:
        /** Отслеживаемая последовательность значений. */
        const T* sequence;

        /** Длина отслеживаемой последовательности значений. */
        int sequence_length;

        /** Кольцевой буфер для сохранения последовательности
         * входных данных. */
        T buffer[buffer_size];

        /** Позиция для добавления данных в буфер. */
        int index;

        /** Текущее состояние: совпадают или нет данные в буфере
         * с ожидаемой последовательностью. */
        bool state;

        /** Указатель на функцию обратной связи. */
        Listener listener;

    public:
        /** Конструктор. */
        SequenceTracker():
            sequence(nullptr),
            sequence_length(0),
            buffer(),
            index(0),
            state(false),
            listener(nullptr)
        {}

        /** Задаёт отслеживаемую последовательность значений. */
        void set_sequence(const T* sequence, int sequence_length) {
            // Устанавливаем ожидаемую последовательность.
            this->sequence = sequence;
            this->sequence_length = sequence? sequence_length : 0;
            // Обновляем состояние.
            update_state();
        }

        /** Задаёт указатель на функцию обратной связи,
         * которая будет вызываться каждый раз, когда
         * после обработки следующего элемента во входной
         * последовательности меняется состояние: есть/нет совпадения.
         */
        void set_listener(const Listener listener) {
            this->listener = listener;
        }

        /** Возвращает true, если содержимое буфера совпадает с ожидаемой
         * последовательностью. */
        bool get_state() const {
            return state;
        }

        /** Обрабатывает следующий элемент. */
        void process(const T input) {
            // Заносим данные в буфер.
            buffer[index] = input;
            index = (index + 1) % buffer_size;
            // Обновляем состояние.
            update_state();
        }

        /** Вызывает функцию обратной связи, если задана. */
        void notify() {
            if (listener) {
                listener(*this);
            }
        }

    private:
        /** Обновляет состояние: если или нет совпадения данных в буфере
         * с ожидаемой последовательностью. При измешении состояния уведомляет
         * подписчиков, вызывая функцию обратной связи. */
        void update_state() {
            // Сравниваем данные в буфере с ожидаемой последовательностью
            // значений.
            bool new_state = match();

            // Если состояние изменилось, то обновляем его
            // и вызываем обработчик.
            if (state != new_state) {
                state = new_state;
                notify();
            }
        }

        /** Возвращает true, если данные во входном буфере совпадают
         * с ожидаемой последовательностью. */
        const bool match() const {
            int i(0);
            int j((buffer_size + (index - sequence_length) % buffer_size)
                    % buffer_size);
            while (i < sequence_length) {
                if (sequence[i] != buffer[j]) {
                    return false;
                }
                ++i;
                j = (j + 1) % buffer_size;
            }
            return true;
        }
};

#endif

