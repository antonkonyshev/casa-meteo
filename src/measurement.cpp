#include "measurement.h"

history_t* history;

history_t* setupHistory() {
    history = (history_t*) malloc(sizeof(history_t));
    history->first = nullptr;
    history->last = nullptr;
    history->length = 0;
    return history;
}

void freeHistory() {
    if (history->first) {
        history_record_t* currentNode = history->first;
        while (currentNode) {
            history_record_t* oldNode = currentNode;
            currentNode = currentNode->next;
            free(oldNode->measurement);
            free(oldNode);
        }
    }
    free(history);
}

history_record_t* appendToHistory(measurement_t* measurement) {
    history_record_t* newNode = (history_record_t*) malloc(sizeof(history_record_t));
    newNode->next = nullptr;
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
            free(oldNode->measurement);
            free(oldNode);
        }
    } else {
        history->first = newNode;
        history->last = newNode;
        history->length = 1;
    }

    return newNode;
}

history_record_t* periodicalAppendToHistory(measurement_t* measurement) {
    if (history->length < HISTORY_LENGTH || measurement->timestamp >= history->last->measurement->timestamp + HISTORY_RECORDS_PERIOD) {
        return appendToHistory(measurement);
    }
    return NULL;
}

history_t* getHistory() {
    return history;
}