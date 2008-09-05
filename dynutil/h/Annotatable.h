/*
 * Copyright (c) 1996-2004 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * This license is for research uses.  For such uses, there is no
 * charge. We define "research use" to mean you may freely use it
 * inside your organization for whatever purposes you see fit. But you
 * may not re-distribute Paradyn or parts of Paradyn, in any form
 * source or binary (including derivatives), electronic or otherwise,
 * to any other organization or entity without our permission.
 * 
 * (for other uses, please contact us at paradyn@cs.wisc.edu)
 * 
 * All warranties, including without limitation, any warranty of
 * merchantability or fitness for a particular purpose, are hereby
 * excluded.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * Even if advised of the possibility of such damages, under no
 * circumstances shall we (or any other person or entity with
 * proprietary rights in the software licensed hereunder) be liable
 * to you or any third party for direct, indirect, or consequential
 * damages of any character regardless of type of action, including,
 * without limitation, loss of profits, loss of use, loss of good
 * will, or computer failure or malfunction.  You agree to indemnify
 * us (and any other person or entity with proprietary rights in the
 * software licensed hereunder) for any and all liability it may
 * incur to third parties resulting from your use of Paradyn.
 */

// $Id: Annotatable.h,v 1.12 2008/09/05 04:06:08 jaw Exp $

#ifndef _ANNOTATABLE_
#define _ANNOTATABLE_

#include <cstring>
#include <vector>
#include <string>
#include <typeinfo>
#include <assert.h>
#include "dyntypes.h"
#include "util.h"
#include "Serialization.h"

class AnnotatableBase;

using std::vector;


class SerDes;
class SerializerBase;
class SerializerError;
void printSerErr(SerializerError &);
SerializerBase *getSDAnno(std::string &anno_name);
void dumpSDAnno();

template <class T, class ANNO_NAME_T>
bool init_anno_serialization(SerializerBase * = NULL);

class DLLEXPORT_COMMON AnnotatableBase
{
   protected:
      AnnotatableBase();
      ~AnnotatableBase() {}
      typedef struct {
         int anno_id;
#if defined (cap_serialization)
         SerializerBase *anno_serializer;
#endif
      } anno_info_t;

   private:
      static std::string emptyString;
      static int number;
      static dyn_hash_map<std::string, anno_info_t> annotationTypes;
      static dyn_hash_map<std::string, int> metadataTypes;
      static dyn_hash_map<std::string, std::string> annotypes_to_typenames;
      static int metadataNum;

   public:

      static int createAnnotationType(std::string &name, const char *tname, SerializerBase *);
      static int getAnnotationType(std::string &name, const char *tname = NULL);
      static SerializerBase *getAnnotationSerializer(std::string &name);
      static  void dumpAnnotationSerializers();
      static std::string &findTypeName(std::string anno_name);
      static int getOrCreateAnnotationType(std::string &anno_name, const char *tname); 

#if 0
      virtual int createMetadata(std::string &name);
      virtual int getMetadata(std::string &name);
#endif

};

template<class T>
class AnnotationSet {
   //  T is a container, so far, always a vector of something else

   typedef dyn_hash_map<int, T*> as_id_map_t;
   typedef dyn_hash_map<char *, as_id_map_t> obj_map_t;
   static obj_map_t sets_by_obj;

   public:
   AnnotationSet() {}
   ~AnnotationSet() {}

   static T *findAnnotationSet(char  *b, int anno_id) 
   {
      typename obj_map_t::iterator iter = sets_by_obj.find(b);
      if (iter == sets_by_obj.end()) {
         //fprintf(stderr, "%s[%d]:  no annotations for object %p\n", FILE__, __LINE__, b);
         return NULL;
      }

      as_id_map_t &anno_sets_by_id = iter->second;
      typename as_id_map_t::iterator nm_iter = anno_sets_by_id.find(anno_id);
      if (nm_iter == anno_sets_by_id.end()) {
         //fprintf(stderr, "%s[%d]:  no annotations of type %d for object %p\n", 
         //      FILE__, __LINE__, anno_id, b);
         return NULL;
      }

      return nm_iter->second;
   }

   static T *getAnnotationSet(char *b, int anno_id)
   {
      T *it = NULL;

      if (NULL == (it = findAnnotationSet(b, anno_id))) {
         //fprintf(stderr, "%s[%d]:  creating new annotation set for %p id %d\n", 
         //FILE__, __LINE__, b,anno_id);
         //fprintf(stderr, "%s[%d]:  sets_by_obj.size() = %d\n", 
         //FILE__, __LINE__, sets_by_obj.size());

         //  operator[] should create new elements if they don't exist

         as_id_map_t &id_map = sets_by_obj[b];

         //  wee sanity check...
         if (sets_by_obj.end() == sets_by_obj.find(b)) {
            fprintf(stderr, "%s[%d]:  FATAL!? &sets_by_obj = %p, b = %p\n", 
                  FILE__, __LINE__, &sets_by_obj, b);
            fprintf(stderr, "%s[%d]:  sets_by_obj.size() = %d\n", 
                  FILE__, __LINE__, sets_by_obj.size());
         }

         // sanity check, make sure this map does not contain <name> already

         typename as_id_map_t::iterator iter = id_map.find(anno_id);

         if (iter != id_map.end()) {

            //  we have already created a container for this annotation type, for this particular
            //  object, just return it.

            return iter->second;

         //   fprintf(stderr, "%s[%d]:  WARNING:  exists:  object %p, id %d, size %d container = %s\n", FILE__, __LINE__, b, anno_id, iter->second->size(),typeid(T).name());
         }

         it = new T();
         id_map[anno_id] = it;
      }

      return it;
   }


   static bool removeAnnotationSet(char *b, int anno_id)
   {
      typename obj_map_t::iterator iter = sets_by_obj.find(b);
      if (iter == sets_by_obj.end())
         return false;

      as_id_map_t *anno_sets_by_id_ptr  = iter->second;
      assert(anno_sets_by_id_ptr);
      as_id_map_t &anno_sets_by_id = *anno_sets_by_id_ptr;

      typename as_id_map_t::iterator nm_iter = anno_sets_by_id.find(anno_id);
      if (nm_iter == anno_sets_by_id.end()) {
         // sanity check that this map is not empty
         assert(anno_sets_by_id.size());
         return false;
      }

      delete nm_iter->second;
      anno_sets_by_id.erase(nm_iter);
      
      //  if we just got rid of the last element of the map, get rid of the map
      //  (within a map) as well.

      if (!anno_sets_by_id.size()) {
         fprintf(stderr, "%s[%d]:  DELETING\n", FILE__, __LINE__);
         delete anno_sets_by_id_ptr;
         sets_by_obj.erase(iter);
      }

      return true;
   }
};

template< class T > dyn_hash_map<char *, dyn_hash_map<int, T*> >
AnnotationSet< T >::sets_by_obj;

template <class T, class ANNOTATION_NAME_T, bool SERIALIZABLE = false>
class Annotatable : public AnnotatableBase
{
   public:
      Annotatable(SerializerBase *serializer) :
         AnnotatableBase()
      {
#if defined (cap_serialization)
         static bool did_init = false;
         if (SERIALIZABLE && !did_init) {
            init_anno_serialization<T, ANNOTATION_NAME_T>(serializer);
            did_init = true;
         }
#endif
      }
      Annotatable() :
         AnnotatableBase()
      {
         //  NULL default value required for default ctors of derived objects
         //  thus all code that uses serializer must check for its existence
#if defined (cap_serialization)
         static bool did_init = false;
         if (SERIALIZABLE && !did_init) {
            init_anno_serialization<T, ANNOTATION_NAME_T>(NULL);
            did_init = true;
         }
#endif
      }
      ~Annotatable() 
      {
      }

      Annotatable(const Annotatable<T, ANNOTATION_NAME_T, SERIALIZABLE> &/*src*/) :
         AnnotatableBase()
      {/*hrm deep copy here or no?*/}

      typedef typename std::vector<T> Container_t;
      Container_t *initAnnotations()
      {
         Container_t *v = NULL;
         std::string anno_name = typeid(ANNOTATION_NAME_T).name();

         int anno_id = getOrCreateAnnotationType(anno_name, typeid(T).name());
         if (anno_id == -1) {
            fprintf(stderr, "%s[%d]:  failed to getOrCreateAnnotation type %s\n", 
                  FILE__, __LINE__, anno_name.c_str());
            return NULL;
         }

         v = AnnotationSet<Container_t>::getAnnotationSet((char *)this, anno_id);
         if (v) {
            //fprintf(stderr, "%s[%d]:  annotation set already exists for %s/%p\n", 
            //      FILE__, __LINE__, anno_name.c_str(), this);
            return v;
         }

         if (!v) {
            fprintf(stderr, "%s[%d]:  malloc problem\n", FILE__, __LINE__);
            abort();
            return NULL;
         }

         return v;
      }


      bool addAnnotation(T it)
      {
         Container_t *v = NULL;
         std::string anno_name = typeid(ANNOTATION_NAME_T).name();

         int anno_id = getOrCreateAnnotationType(anno_name, typeid(T).name());
         if (anno_id == -1) {
            fprintf(stderr, "%s[%d]:  failed to getOrCreateAnnotation type %s\n", 
                  FILE__, __LINE__, anno_name.c_str());
            return false;
         }

         v = AnnotationSet<std::vector<T> >::findAnnotationSet((char *)this, anno_id);
         if (!v)
            if (NULL == ( v = initAnnotations())) {
               fprintf(stderr, "%s[%d]:  bad annotation type\n", FILE__, __LINE__);
               return false;
            }

         if (!v) {
            fprintf (stderr, "%s[%d]:  initAnnotations failed\n", FILE__, __LINE__);
            return false;
         }

         v->push_back(it);

#if defined (cap_serialization)
         if (SERIALIZABLE) {

            fprintf(stderr, "%s[%d]:  serializing annotation here for %s\n", 
                  FILE__, __LINE__, anno_name.c_str());

            SerializerBase *sd_bin = getSDAnno(anno_name);

            if (!sd_bin) {
               //fprintf(stderr, "%s[%d]:  ERROR:  no serializer listed for type %s, have:\n",
                //     FILE__, __LINE__, anno_name.c_str());
               //dumpSDAnno();

               fprintf(stderr, "%s[%d]:  ERROR:  no serializer listed for type %s\n",
                     FILE__, __LINE__, anno_name.c_str());

               return false;
            }

            if (!gtranslate_w_err(sd_bin, /*(Serializable &)*/ it, NULL)) {
               fprintf(stderr, "%s[%d]:  gtranslate_w_err failed\n", FILE__, __LINE__);
               return false;
            }

            fprintf(stderr, "%s[%d]:  \tserialized annotation here for %s\n", 
                  FILE__, __LINE__, anno_name.c_str());
         }
#endif
         return true;
      }

      void clearAnnotations()
      {
         Container_t *v = NULL;
         std::string anno_name = typeid(ANNOTATION_NAME_T).name();
         int anno_id = getOrCreateAnnotationType(anno_name, typeid(T).name());
         if (anno_id == -1) {
            return;
         }
         v = AnnotationSet<std::vector<T> >::findAnnotationSet((char *)this, anno_id);
         if (v)
            v->clear();
      }

      unsigned size() const 
      {
         Container_t *v = NULL;
         std::string anno_name = typeid(ANNOTATION_NAME_T).name();
         int anno_id = getOrCreateAnnotationType(anno_name, typeid(T).name());
         if (anno_id == -1) {
            fprintf(stderr, "%s[%d]:  failed to getOrCreateAnnotation type %s\n", 
                  FILE__, __LINE__, anno_name.c_str());
            return 0;
         }

         //  ahhh the things we do to get rid of constness
         const Annotatable<T, ANNOTATION_NAME_T,  SERIALIZABLE> *thc = this; 
         Annotatable<T, ANNOTATION_NAME_T,  SERIALIZABLE> *th  
            = const_cast<Annotatable<T, ANNOTATION_NAME_T,  SERIALIZABLE> *> (thc);

         v = AnnotationSet<Container_t>::findAnnotationSet((char *)th, anno_id);
         if (!v) {
            //fprintf(stderr, "%s[%d]:  no annotation set for id %d\n", FILE__, __LINE__, anno_id);
            return 0;
         }
         return v->size();
      }

      //  so called getDataStructure in case we generalize beyond vectors for annotations
      std::vector<T> &getDataStructure() 
      {
         // use with caution since this function will assert upon failure 
         // (it has no way to return errors)
         // when in doubt, check size() first.
         Container_t *v = NULL;
         std::string anno_name = typeid(ANNOTATION_NAME_T).name();
         int anno_id = getOrCreateAnnotationType(anno_name, typeid(T).name());
         if (anno_id == -1) {
            fprintf(stderr, "%s[%d]:  failed to getOrCreateAnnotation type %s\n", 
                  FILE__, __LINE__, anno_name.c_str());
            assert(0);
         }
         v = AnnotationSet<Container_t>::findAnnotationSet((char *)this, anno_id);
         if (!v)
            if (NULL == (v = initAnnotations())) {
               fprintf(stderr, "%s[%d]:  failed to init annotations here\n", 
                     FILE__, __LINE__);
               assert(0);
            }
         return *v;
      }

      T &getAnnotation(unsigned index) const
      {
         Container_t *v = NULL;
         std::string anno_name = typeid(ANNOTATION_NAME_T).name();
         int anno_id = getOrCreateAnnotationType(anno_name, typeid(T).name());
         if (anno_id == -1) {
            fprintf(stderr, "%s[%d]:  failed to getOrCreateAnnotation type %s\n", 
                  FILE__, __LINE__, anno_name.c_str());
            assert(0);
         }

         //  ahhh the things we do to get rid of constness
         const Annotatable<T, ANNOTATION_NAME_T,  SERIALIZABLE> *thc = this; 
         Annotatable<T, ANNOTATION_NAME_T,  SERIALIZABLE> *th  
            = const_cast<Annotatable<T, ANNOTATION_NAME_T,  SERIALIZABLE> *> (thc);

         v = AnnotationSet<Container_t>::findAnnotationSet((char *)th, anno_id);
         if (!v) {
            fprintf(stderr, "%s[%d]:  cannot find annotation set for anno type %s\n", 
                  FILE__, __LINE__, anno_name.c_str());
            v = AnnotationSet<Container_t>::getAnnotationSet((char *)th, anno_id);
         }
         assert(v);
         if (index >= v->size()) {
            fprintf(stderr, "%s[%d]:  FIXME:  index = %d -- size = %d\n", 
                  FILE__, __LINE__, index, v->size());
         }
         assert(index < v->size());
         return (*v)[index];
      }

      T &operator[](unsigned index) const {return getAnnotation(index);}

};

template <class S, class T>
bool annotate(S *obj, T &anno, std::string anno_name)
{
   assert(obj);
   char *tobj = (char *) obj;
   std::vector<T> *v = NULL;

   int anno_id = AnnotatableBase::getOrCreateAnnotationType(anno_name, typeid(T).name());

   if (-1 == anno_id) {
      fprintf(stderr, "%s[%d]:  failed to getOrCreateAnnotation type %s\n", 
            FILE__, __LINE__, anno_name.c_str());
      return false;
   }

   v = AnnotationSet<std::vector<T> >::getAnnotationSet(tobj, anno_id);

   if (!v) {
      fprintf(stderr, "%s[%d]:  failed to init annotations here\n", 
            FILE__, __LINE__);
      assert(0);
   }

   v->push_back(anno);
   return true;
}

template <class S, class T>
std::vector<T> *getAnnotations(S *obj, std::string anno_name)
{
   //  does not allocate anything, should return NULL if there are none
   assert(obj);
   char *tobj = (char *) obj;
   std::vector<T> *v = NULL;

   int anno_id = AnnotatableBase::getOrCreateAnnotationType(anno_name, typeid(T).name());

   if (-1 == anno_id) {
      fprintf(stderr, "%s[%d]:  failed to getOrCreateAnnotation type %s\n", 
            FILE__, __LINE__, anno_name.c_str());
      return false;
   }

   v = AnnotationSet<std::vector<T> >::findAnnotationSet(tobj, anno_id);

   return v;
}

#endif
