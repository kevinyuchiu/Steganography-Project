//
//  HashTable.h
//  Steg-o-matic
//
//  Created by Anthony Lai on 3/8/15.
//  Copyright (c) 2015 Yu-Kuan (Anthony) Lai. All rights reserved.
//

#ifndef Steg_o_matic_HashTable_h
#define Steg_o_matic_HashTable_h

template <typename KeyType, typename ValueType>
class HashTable
{
public:
    HashTable(unsigned int numBuckets, unsigned int capacity);
    ~HashTable();
    bool isFull() const;
    bool set(const KeyType& key, const ValueType& value, bool permanent = false);
    bool get(const KeyType& key, ValueType& value) const;
    bool touch(const KeyType& key);
    bool discard(KeyType& key, ValueType& value);
    
private:
    //  We prevent a HashTable from being copied or assigned by declaring the
    //  copy constructor and assignment operator private and not implementing them.
    HashTable(const HashTable&);
    HashTable& operator=(const HashTable&);
    
    struct Node {
        KeyType key;
        ValueType value;
        Node* prev;
        Node* next;
        Node* orderPrev;
        Node* orderNext;
        bool isPermanent;
    };
    
    //  private member functions
    unsigned int getBucketNum(const KeyType& key) const;
        //  get the bucket number using computeHash and module opertator
    
    bool getNodePtr(const KeyType& key, Node*& ptr) const;
        //  if key is found in the HashTable, return true with ptr pointing to the Node
        //  otherwise, return false with ptr points to the last Node in the linked list
    
    void updateRecentList(Node* ptrToUpdate);
    
    Node** m_buckets;    //  dynamically allocated array
    unsigned int m_nBuckets;
    unsigned int m_nCapacity;
    unsigned int m_nUsed;
    Node* m_leastRecent;
    Node* m_mostRecent;
    
};

//  TO_DO implement/move this to cpp
//unsigned int computeHash(std::string key);

template <typename KeyType, typename ValueType>
HashTable<KeyType, ValueType>::HashTable(unsigned int numBuckets, unsigned int capacity)
 :m_nBuckets(numBuckets), m_nCapacity(capacity), m_nUsed(0)
{
    m_buckets = new Node*[numBuckets];
    m_leastRecent = nullptr;
    m_mostRecent = nullptr;
    
    //  create a dummy Node in each bucket
    for (int i = 0; i < numBuckets; i++) {
        m_buckets[i] = new Node;
        m_buckets[i]->prev = nullptr;
        m_buckets[i]->next = nullptr;
        m_buckets[i]->orderPrev = nullptr;
        m_buckets[i]->orderNext = nullptr;
        m_buckets[i]->isPermanent = true;
    }
}

//  TO_DO destructor here
template <typename KeyType, typename ValueType>
HashTable<KeyType, ValueType>::~HashTable()
{
    for (int i = 0; i < m_nBuckets; i++) {
        Node* ptrToDelete;
        Node* ptr = m_buckets[i];
        while (ptr != nullptr) {
            ptrToDelete = ptr;
            ptr = ptr->next;
            delete ptrToDelete;
        }
    }
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::isFull() const
{
    return m_nUsed == m_nCapacity;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::set(const KeyType& key, const ValueType& value, bool permanent)
{
    Node* keyPtr;
    bool doesNodeExist = getNodePtr(key, keyPtr);
    if (!doesNodeExist && isFull())
        //  key not in the HashTable and HashTable is full
        return false;
    
    if (!doesNodeExist) {
        //  key not in the HashTable and keyPtr points to the last Node in the linked list
        Node* newlyAdded = new Node;
        keyPtr->next = newlyAdded;
        newlyAdded->prev = keyPtr;
        newlyAdded->next = nullptr;
        newlyAdded->key = key;
        newlyAdded->value = value;
        m_nUsed++;
        
        if (!permanent) {
            newlyAdded->isPermanent = false;
            if (m_leastRecent == nullptr) {
                //  first Node to track (first non-permanent)
                m_leastRecent = m_mostRecent = newlyAdded;
                newlyAdded->orderPrev = newlyAdded->orderNext = nullptr;
            } else {
                m_mostRecent->orderNext = newlyAdded;
                newlyAdded->orderPrev = m_mostRecent;
                newlyAdded->orderNext = nullptr;
                m_mostRecent = newlyAdded;
            }
        } else {
            newlyAdded->isPermanent = true;
        }
    } else {
        //  key already in the table
        keyPtr->value = value;
        if (!keyPtr->isPermanent) {
            //  not permanent, must be moved to the most recently-written list
            updateRecentList(keyPtr);
        }
    }
    return true;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::get(const KeyType& key, ValueType& value) const
{
    Node* keyPtr;
    if (!getNodePtr(key, keyPtr))
        //  key not in the HashTable
        return false;
    else {
        value = keyPtr->value;
        return true;
    }
}


template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::touch(const KeyType& key)
{
    Node* keyPtr;
    if (!getNodePtr(key, keyPtr) || keyPtr->isPermanent)
        //  key not in the HashTable or key is permanent
        return false;
    
    updateRecentList(keyPtr);
    return true;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::discard(KeyType& key, ValueType& value)
{
    if (m_leastRecent == nullptr)
        //  none of the Node is non-permanent
        return false;
    Node* ptrToDelete = m_leastRecent;
    key = ptrToDelete->key;
    value = ptrToDelete->value;
    
    //  update recently-written list
    if (ptrToDelete->orderNext == nullptr) {
        //  this is the only non-permanent Node
        m_leastRecent = m_mostRecent = nullptr;
    } else {
        //  there is/are other non-permanent Node(s)
        m_leastRecent = ptrToDelete->orderNext;
        m_leastRecent->orderPrev = nullptr;
    }
    
    //  update linked list in the bucket
    if (ptrToDelete->next == nullptr) {
        //  last Node in the linked list
        ptrToDelete->prev->next = nullptr;
    } else {
        ptrToDelete->prev->next = ptrToDelete->next;
        ptrToDelete->next->prev = ptrToDelete->prev;    //  there's always a Node before (perhaps dummy)
    }
    
    //  delete Node
    delete ptrToDelete;
    m_nUsed--;
    return true;
}

//  private member/helper function
template <typename KeyType, typename ValueType>
unsigned int HashTable<KeyType, ValueType>::getBucketNum(const KeyType& key) const
{
    unsigned int computeHash(KeyType);  //  prototype
    unsigned int hashNum = computeHash(key);
    unsigned int bucketNum = hashNum % m_nBuckets;
    return bucketNum;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::getNodePtr(const KeyType& key, Node*& ptr) const
{
    unsigned int keyBucketNum = getBucketNum(key);
    ptr = m_buckets[keyBucketNum];
    for (;;) {  //  don't need to check in the first step because there are dummy Nodes
        if (ptr->key == key && ptr->prev != nullptr)
            //  the second condition confirms that this is not the dummy Node
            return true;
        if (ptr->next == nullptr)
            //  this is the end of the linked list
            //  Node not found
            return false;
        else
            ptr = ptr->next;
    }
}
template <typename KeyType, typename ValueType>
void HashTable<KeyType, ValueType>::updateRecentList(Node* ptrToUpdate) {
    if (m_mostRecent != ptrToUpdate) {
        //  more than one non-permanent Node in the recenlty-written list
        //  and this Node is not already the most recent one
        m_mostRecent->orderNext = ptrToUpdate;
        
        if (ptrToUpdate->orderPrev != nullptr) {
            //  && ptrToUpdate->orderNext != nullptr (should not need to check this)
            //  this Node is somewhere in the middle of the recently-written list
            ptrToUpdate->orderPrev->orderNext = ptrToUpdate->orderNext;
            ptrToUpdate->orderNext->orderPrev = ptrToUpdate->orderPrev;
        } else {
            // this Node is the first one (least recent one)
            m_leastRecent = ptrToUpdate->orderNext;
            m_leastRecent->orderPrev = nullptr;
        }
        
        ptrToUpdate->orderPrev = m_mostRecent;
        ptrToUpdate->orderNext = nullptr;
        m_mostRecent = ptrToUpdate;
    }
}


#endif
