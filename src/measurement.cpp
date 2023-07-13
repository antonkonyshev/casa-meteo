#include "measurement.h"

history_t* history;

void cleanHistory() {
    if (history->first) {
        history_record_t* currentNode = history->first;
        while (currentNode) {
            history_record_t* oldNode = currentNode;
            currentNode = currentNode->next;
            delete oldNode->measurement;
            delete oldNode;
        }
    }
    delete history;
}

history_record_t* appendToHistory(measurement_t* measurement) {
    if (!history) {
        history = new history_t;
    }
    history_record_t* newNode = new history_record_t;
    newNode->measurement = measurement;

    if (history->last) {
        history->last->next = newNode;
        newNode->prev = history->last;
        history->last = newNode;
        history->length += 1;
        if (history->length > HISTORY_LENGTH) {
            history_record_t* oldNode = history->first;
            history->first = oldNode->next;
            history->length -= 1;
            delete oldNode->measurement;
            delete oldNode;
        }
    } else {
        history->first = newNode;
        history->last = newNode;
        history->length = 1;
    }

    return newNode;
}

history_record_t* periodicalAppendToHistory(measurement_t* measurement) {
    if (!history || history->length < HISTORY_LENGTH || measurement->timestamp >= history->last->measurement->timestamp + HISTORY_RECORDS_PERIOD) {
        return appendToHistory(measurement);
    }
    return NULL;
}

history_t* getHistory() {
    return history;
}