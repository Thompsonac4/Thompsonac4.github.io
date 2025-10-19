#!/usr/bin/env python
# coding: utf-8

# In[ ]:


from pymongo import MongoClient
from bson.objectid import ObjectId

class AnimalShelter(object):
    """ CRUD operations for Animal collection in MongoDB """

    def __init__(self,USER,PASS,HOST,PORT,DB,COL):
        #
        # Initialize Connection
        # Values are passed from the test file
        #
        try:
            self.client = MongoClient('mongodb://%s:%s@%s:%d' % (USER,PASS,HOST,PORT))
            self.database = self.client['%s' % (DB)]
            self.collection = self.database['%s' % (COL)]
        except PyMongoError as e:
            raise Exception(f"Failed to connect to MongoDB: {e}")

# Complete this create method to implement the C in CRUD.
    def create(self, data):
        if data is not None and isinstance(data, dict):
            try:
                # Insert the document into the collection
                result = self.database.animals.insert_one(data)  # data should be dictionary
                return result.acknowledged # Returns tru if insert was acknowledged
            except PyMongoError as e:
                print(f"Error Inserting Document: {e}")
                return False
        else:
            # Error if input is invalid
            raise ValueError("Invalid input: data must be a non-empty dictionary.")

# Create method to implement the R in CRUD.
    def read(self, query):
        if query is not None and isinstance(query, dict):
            try:
                # Uses find() to retrieve matching documents
                results = self.collection.find(query)
                return list(results)
            except:
                print(f"Error Inserting Document: {e}")
                return []
        else:
            # Error if input is invalid
            raise ValueError("Invalid input: data must be a dictionary.")
            
# Update method to implement the U in CRUD
    def update(self, query, new_values):
        if isinstance(query, dict) and isinstance(new_values, dict):
            try:
                # Use update_many to apply to all matching documents
                result = self.collection.update_many(query, {"$set": new_values})
                return result.modified_count # Number of Documents Updated
            except Exception as e:
                print(f"Error Updating Document: {e}")
                return 0
        else:
            # Error if input is invalid
            raise ValueError("Query and new_values must both be dictionaries.")
            
# Delete method to implement the D in CRUD    
    def delete(self, query):
        if isinstance(query, dict):
            try:
                # Use delete_many to remove all matching the query
                result = self.collection.delete_many(query)
                return result.deleted_count # Number of Documents Deleted
            except Exception as e:
                print(f"Error deleting Document: {e}")
                return 0
        else:
            # Error if input is invalid
            raise ValueError("Query must be a dictionary.")