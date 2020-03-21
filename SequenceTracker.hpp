#ifndef _SEQUENCE_TRACKER_HPP_
#define _SEQUENCE_TRACKER_HPP_

/** Шаблон класса, предназначенного для отслеживания определённой
 *  последовательности значений во входных данных, скармливаемых
 *  ему по одному значению за раз.
 */
template <typename T>
class SequenceTracker {
    public:
        /** Тип указателя на функцию обратной связи, вызываемой
         * при изменении состояния: есть или нет совпадения. */
        using Listener = void (*) (const SequenceTracker<T>& sender);

    private:
        /** Отслеживаемая последовательность значений. */
        const T* sequence;

        /* Длина отслеживаемой последовательности. */
        int sequence_length;

        /** Текущая позиция совпадения. */
        int position;

        /** Функция для обратной связи при изменении состояния. */ 
        Listener listener;

    public:
        /** Конструктор. */
        SequenceTracker():
            sequence(nullptr),
            sequence_length(0),
            position(0),
            listener(nullptr)
        {}

        /** Задает отслеживаемую последовательность значений. */
        void set_sequence(const T* sequence, const int sequence_length) {
            this->sequence = sequence;
            this->sequence_length = sequence? sequence_length : 0;
            this->position = 0;
        }

        /** Задаёт функцию для обратной связи при изменении состояния. */
        void set_listener(const Listener listener) {
            this->listener = listener;
        }

        /** Обрабатывает входное значение. */
        void process(const T input) {
            if (!sequence) {
                return;
            }
            
            bool old_state = get_state();
            
            if (position == sequence_length) {
                position = 0;
            }
            position = (sequence[position] == input)? (position + 1) : 0;
            
            bool new_state = get_state();
            if (old_state != new_state) {
                notify();
            }
        }

        /** Возвращает состояние: есть совпадение с отслеживаемой
         * последовательностью или нет. */
        bool get_state() const {
            return sequence && (position == sequence_length);
        }

        /** Вызывает функцию обратной связи, если задана. */
        void notify() const {
            if (listener) {
                listener(*this);
            }
        }
};

#endif

