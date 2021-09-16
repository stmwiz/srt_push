//
// Created by TBD on 3/2/21.
//

#ifndef XLIVE_TEMPLATE_HPP
#define XLIVE_TEMPLATE_HPP

#include <type_traits>

namespace xlab {

namespace NSTemplate {

#pragma mark - NotFound
struct NotFound;


#pragma mark - TypeWrap
template<typename T>
struct TypeWrap {
    using type = T;
};


#pragma mark - PairType
template<typename T1, typename T2>
struct PairType_ {
    using first = T1;
    using second = T2;
};

template<typename T1, typename T2>
using PairType = PairType_<T1, T2>;


#pragma mark - TypeList
template<typename ...T>
struct TypeList_;

template<>
struct TypeList_<> {
    static constexpr size_t length = 0;
};

template<typename Cur, typename ...T>
struct TypeList_<Cur, T...> {
    static constexpr size_t length = 1 + TypeList_<T...>::length;
};

template<typename ...T>
using TypeList = TypeList_<T...>;


#pragma mark - TypeDict
template<typename ...PairTypes>
struct TypeDict_;

template<>
struct TypeDict_<> {
    static constexpr size_t count = 0;
};

template<typename K, typename V, typename ...PairTypes>
struct TypeDict_<PairType<K, V>, PairTypes...> {
    static constexpr size_t count = 1 + TypeDict_<PairTypes...>::count;
};

template<typename ...PairTypes>
using TypeDict = TypeDict_<PairTypes...>;


#pragma mark - AndValue
template<bool cur, typename TNext>
static constexpr bool AndValue = false;

template<typename TNext>
static constexpr bool AndValue<true, TNext> = TNext::val;


#pragma mark - OrValue
template<bool cur, typename TNext>
constexpr static bool OrValue = true;

template<typename TNext>
constexpr static bool OrValue<false, TNext> = TNext::value;


#pragma mark - ConditionType
template<bool condition, typename TCur, typename TNext>
struct ConditionType_ {
    using type = typename TCur::type;
};

template<typename TCur, typename TNext>
struct ConditionType_<false, TCur, TNext> {
    using type = typename TNext::type;
};

template<bool condition, typename TCur, typename TNext>
using ConditionType = typename ConditionType_<condition, TCur, TNext>::type;


#pragma mark - ContainType
template<typename FindType, typename... T>
struct ContainType_ {
    static constexpr bool value = false;
};

template<typename FindType, typename CurType, typename... T>
struct ContainType_<FindType, CurType, T...> {
    constexpr static bool value = OrValue<std::is_same<FindType, CurType>::value, ContainType_<FindType, T...>>;
};

template<typename FindType, typename... T>
struct ContainType_<FindType, TypeList<T...>> {
    constexpr static bool value = ContainType_<FindType, T...>::value;
};

template<typename FindType, typename... T>
constexpr static bool ContainType = ContainType_<FindType, T...>::value;


#pragma mark - Type2Pos
template<typename FindType, size_t Index, typename CurType, typename ...T>
struct Type2Pos_ {
    static constexpr size_t value = Type2Pos_<FindType, Index + 1, T...>::value;
};

template<typename FindType, size_t Index, typename ...T>
struct Type2Pos_<FindType, Index, FindType, T...> {
    static constexpr size_t value = Index;
};

template<typename FindType, size_t Index, typename ...T>
struct Type2Pos_<FindType, Index, TypeList<T...>> {
    static constexpr size_t value = Type2Pos_<FindType, Index, T...>::value;
};

template<typename FindType, typename ...T>
constexpr size_t Type2Pos = Type2Pos_<FindType, 0, T...>::value;


#pragma mark - Pos2Type
template<size_t Pos, typename ...T>
struct Pos2Type_ {
    static_assert(Pos != 0, "error");
};

template<size_t Pos, typename CurType, typename ...T>
struct Pos2Type_<Pos, CurType, T...> {
    using type = ConditionType<
        Pos == 0,
        TypeWrap<CurType>,
        Pos2Type_<Pos - 1, T...>
    >;
};

template<size_t Pos, typename ...T>
struct Pos2Type_<Pos, TypeList<T...>> {
    using type = typename Pos2Type_<Pos, T...>::type;
};

template<size_t Pos, typename ...T>
using Pos2Type = typename Pos2Type_<Pos, T...>::type;


#pragma mark - TypeListReplace
template<size_t index, typename ...T>
struct TypeListReplace_ {
    static_assert(index != 0, "error");
};

template<size_t index, typename ReplaceType, typename ...Pre, typename CurType, typename ...Post>
struct TypeListReplace_<index, ReplaceType, TypeList<Pre...>, TypeList<CurType, Post...>> {
    using type = ConditionType<
        index == 0,
        TypeWrap<TypeList<Pre..., ReplaceType, Post...>>,
        TypeListReplace_<index - 1, ReplaceType, TypeList<Pre..., CurType>, TypeList<Post...>>
    >;
};

template<typename OriginTypeList, size_t index, typename ReplaceType>
using TypeListReplace = typename TypeListReplace_<index, ReplaceType, TypeList<>, OriginTypeList>::type;


#pragma mark - TypeListCat
template<typename ...T>
struct TypeListCat_;

template<typename ...T>
struct TypeListCat_<TypeList_<T...>> {
    using type = TypeList<T...>;
};

template<typename ...T1, typename ...T2, typename ...OtherTypeLists>
struct TypeListCat_<TypeList<T1...>, TypeList<T2...>, OtherTypeLists...> {
    using type = typename TypeListCat_<TypeList<T1..., T2...>, OtherTypeLists...>::type;
};

template<typename ...TypeList>
using TypeListCat = typename TypeListCat_<TypeList...>::type;


#pragma mark - TypeListAppend
template<typename ...T>
struct TypeListAppend_;

template<typename ...T, typename AppendType>
struct TypeListAppend_<TypeList<T...>, AppendType> {
    using type = TypeList<T..., AppendType>;
};

template<typename PreTypeList, typename AppendType>
using TypeListAppend = typename TypeListAppend_<PreTypeList, AppendType>::type;


#pragma mark - TypeListInsert
template<size_t index, typename ...T>
struct TypeListInsert_ {
    static_assert(index != 0, "error");
};

template<typename InsertType, typename ...Pre, typename ...Post>
struct TypeListInsert_<0, InsertType, TypeList<Pre...>, TypeList<Post...>> {
    using type = TypeList<Pre..., InsertType, Post...>;
};

template<size_t index, typename InsertType, typename ...Pre, typename Next, typename ...Post>
struct TypeListInsert_<index, InsertType, TypeList<Pre...>, TypeList<Next, Post...>> {
    using type = typename TypeListInsert_<index - 1, InsertType, TypeList<Pre..., Next>, TypeList<Post...>>::type;
};

template<typename OriginTypeList, size_t index, typename InsertType>
using TypeListInsert = typename TypeListInsert_<index, InsertType, TypeList<>, OriginTypeList>::type;


#pragma mark - TypeListRemove
template<size_t index, typename ...T>
struct TypeListRemove_ {
    static_assert(index != 0, "error");
};

template<typename ...Pre, typename CurType, typename ...Post>
struct TypeListRemove_<0, TypeList<Pre...>, TypeList<CurType, Post...>> {
    using type = TypeList<Pre..., Post...>;
};

template<size_t index, typename ...Pre, typename Next, typename ...Post>
struct TypeListRemove_<index, TypeList<Pre...>, TypeList<Next, Post...>> {
    using type = typename TypeListRemove_<index - 1, TypeList<Pre..., Next>, TypeList<Post...>>::type;
};

template<typename OriginTypeList, size_t index>
using TypeListRemove = typename TypeListRemove_<index, TypeList<>, OriginTypeList>::type;


#pragma mark - TypeListRemoveType
template<bool RemoveFirst, typename ...T>
struct TypeListRemoveType_;

template<bool RemoveFirst, typename Remove, typename ...Pre, typename ...Post>
struct TypeListRemoveType_<RemoveFirst, Remove, TypeList<Pre...>, TypeList<Remove, Post...>> {
    using type = ConditionType<
        RemoveFirst,
        TypeWrap<TypeList<Pre..., Post...>>,
        TypeListRemoveType_<RemoveFirst, Remove, TypeList<Pre...>, TypeList<Post...>>
    >;
};

template<bool RemoveFirst, typename Remove, typename ...Pre>
struct TypeListRemoveType_<RemoveFirst, Remove, TypeList<Pre...>, TypeList<>> {
    using type = TypeList<Pre...>;
};

template<bool RemoveFirst, typename Remove, typename ...Pre, typename Next, typename ...Post>
struct TypeListRemoveType_<RemoveFirst, Remove, TypeList<Pre...>, TypeList<Next, Post...>> {
    using type = typename TypeListRemoveType_<RemoveFirst, Remove, TypeList<Pre..., Next>, TypeList<Post...>>::type;
};

template<typename OriginTypeList, typename Remove, bool RemoveFirst = true>
using TypeListRemoveType = typename TypeListRemoveType_<RemoveFirst, Remove, TypeList<>, OriginTypeList>::type;


#pragma mark - TypeListRemoveDuplicate
template<typename ...T>
struct TypeListRemoveDuplicate_;

template<typename ...Pre>
struct TypeListRemoveDuplicate_<TypeList<Pre...>, TypeList<>> {
    using type = TypeList<Pre...>;
};

template<typename ...Pre, typename Next, typename ...Post>
struct TypeListRemoveDuplicate_<TypeList<Pre...>, TypeList<Next, Post...>> {
    using type = typename TypeListRemoveDuplicate_<
        ConditionType<
            ContainType<Next, Pre...>,
            TypeWrap<TypeList<Pre...>>,
            TypeWrap<TypeList<Pre..., Next>>
        >,
        TypeList<Post...>
    >::type;
};

template<typename OriginTypeList>
using TypeListRemoveDuplicate = typename TypeListRemoveDuplicate_<TypeList<>, OriginTypeList>::type;


#pragma mark - TypeDictInsert
template<typename ...T>
struct TypeDictInsert_;

template<typename ...Checked, typename InsertKey, typename InsertValue, typename CurKey, typename CurValue, typename ...Remain>
struct TypeDictInsert_<TypeDict<Checked...>, PairType<InsertKey, InsertValue>, TypeDict<PairType<CurKey, CurValue>, Remain...>> {
    using type = ConditionType<
        std::is_same_v<InsertKey, CurKey>,
        TypeWrap<TypeDict<Checked..., PairType<InsertKey, InsertValue>, Remain...>>,
        TypeDictInsert_<TypeDict<Checked..., PairType<CurKey, CurValue>>, PairType<InsertKey, InsertValue>, TypeDict<Remain...>>
    >;
};

template<typename ...Checked, typename InsertKey, typename InsertValue>
struct TypeDictInsert_<TypeDict<Checked...>, PairType<InsertKey, InsertValue>, TypeDict<>> {
    using type = TypeDict<Checked..., PairType<InsertKey, InsertValue>>;
};

template<typename OriginDict, typename InsertKey, typename InsertValue>
using TypeDictInsert = typename TypeDictInsert_<TypeDict<>, PairType<InsertKey, InsertValue>, OriginDict>::type;


#pragma mark - TypeDictFromKVPairList
template<typename ...KVPairs>
struct TypeDictFromPairTypes_;

template<typename ...Processed>
struct TypeDictFromPairTypes_<TypeDict<Processed...>, TypeList<>> {
    using type = TypeDict<Processed...>;
};

template<typename ...Processed, typename CurKey, typename CurValue, typename ...Remain>
struct TypeDictFromPairTypes_<TypeDict<Processed...>, TypeList<PairType<CurKey, CurValue>, Remain...>> {
    using type = typename TypeDictFromPairTypes_<
        TypeDictInsert<TypeDict<Processed...>, CurKey, CurValue>,
        TypeList<Remain...>
    >::type;
};

template<typename ...KVPairs>
using TypeDictFromPairTypes = typename TypeDictFromPairTypes_<TypeDict<>, TypeList<KVPairs...>>::type;


#pragma mark - TypeDictFromKeyListAndValueList_
template<typename ...T>
struct TypeDictFromKeyListAndValueList_;

template<typename ...Processed>
struct TypeDictFromKeyListAndValueList_<TypeDict<Processed...>, TypeList<>, TypeList<>> {
    using type = TypeDict<Processed...>;
};

template<typename ...Processed, typename CurKey, typename CurValue, typename ...Keys, typename ...Values>
struct TypeDictFromKeyListAndValueList_<TypeDict<Processed...>, TypeList<CurKey, Keys...>, TypeList<CurValue, Values...>> {
    static_assert(TypeList<Keys...>::length == TypeList<Values...>::length, "Error: Key List and Value List Not Same Length");
    using type = typename TypeDictFromKeyListAndValueList_<
        TypeDictInsert<TypeDict<Processed...>, CurKey, CurValue>,
        TypeList<Keys...>,
        TypeList<Values...>
    >::type;
};

template<typename KeyTypeList, typename ValueTypeList>
using TypeDictFromKeyListAndValueList = typename TypeDictFromKeyListAndValueList_<TypeList<>, KeyTypeList, ValueTypeList>::type;


#pragma mark - TypeDictCat
template<typename ...T>
struct TypeDictCat_;

template<typename ...PairTypes>
struct TypeDictCat_<TypeDict<PairTypes...>, TypeDict<>> {
    using type = TypeDict<PairTypes...>;
};

template<typename ...PairTypes, typename K, typename V, typename ...Remain>
struct TypeDictCat_<TypeDict<PairTypes...>, TypeDict<PairType<K, V>, Remain...>> {
    using type = typename TypeDictCat_<
        TypeDictInsert<TypeDict<PairTypes...>, K, V>,
        TypeDict<Remain...>
    >::type;
};

template<typename ...PairTypes1, typename ...PairTypes2, typename ...OtherDict>
struct TypeDictCat_<TypeDict<PairTypes1...>, TypeDict<PairTypes2...>, OtherDict...> {
    using type = typename TypeDictCat_<
        typename TypeDictCat_<TypeDict<PairTypes1...>, TypeDict<PairTypes2...>>::type,
        OtherDict...
    >::type;
};

template<typename ...TypeDicts>
using TypeDictCat = typename TypeListCat_<TypeDicts...>::type;


#pragma mark - ValueForKey
template<typename FindKeyType, typename ...T>
struct ValueForKey_ {
    static_assert(TypeList<T...>::length != 0, "Not Found KeyType");
};

/// Double List or Type
template<typename FindKeyType, typename CurKey, typename CurValue, typename ...Remain>
struct ValueForKey_<FindKeyType, TypeDict<PairType<CurKey, CurValue>, Remain...>> {
    using type = ConditionType<
        std::is_same_v<FindKeyType, CurKey>,
        TypeWrap<CurValue>,
        ValueForKey_<FindKeyType, TypeDict<Remain...>>
    >;
};

template<typename FindKeyType, typename TypeDict>
using ValueForKey = typename ValueForKey_<FindKeyType, TypeDict>::type;


#pragma mark - FirstKeyForValue
template<typename FindValue, typename ...T>
struct FirstKeyForValue_ {
    using type = NotFound;
};

template<typename FindValue, typename Key, typename Value, typename ...Remain>
struct FirstKeyForValue_<FindValue, TypeDict<PairType<Key, Value>, Remain...>> {
    using type = ConditionType<
        std::is_same_v<FindValue, Value>,
        TypeWrap<Key>,
        FirstKeyForValue_<FindValue, TypeDict<Remain...>>
    >;
};

template<typename FindValue, typename TypeDict>
using FirstKeyForValue = typename FirstKeyForValue_<FindValue, TypeDict>::type;


#pragma mark - AllKeyForValue
template<typename FindValue, typename ...T>
struct AllKeyForValue_ {
    using type = NotFound;
};

template<typename FindValue, typename ...FoundKeys>
struct AllKeyForValue_<FindValue, TypeList<FoundKeys...>, TypeDict<>> {
    using type = TypeList<FoundKeys...>;
};

template<typename FindValue, typename ...FoundKeys, typename Key, typename Value, typename ...Remain>
struct AllKeyForValue_<FindValue, TypeList<FoundKeys...>, TypeDict<PairType<Key, Value>, Remain...>> {
    using type = typename AllKeyForValue_<
        FindValue,
        ConditionType<
            std::is_same_v<FindValue, Value>,
            TypeWrap<TypeList<FoundKeys..., Key>>,
            TypeWrap<TypeList<FoundKeys...>>
        >,
        TypeDict<Remain...>
    >::type;
};

template<typename FindValue, typename TypeDict>
using AllKeyForValue = typename AllKeyForValue_<FindValue, TypeList<>, TypeDict>::type;


#pragma mark - TypeDictAllKeys
template<typename ...T>
struct TypeDictAllKeys_;

template<typename ...CollectKeys>
struct TypeDictAllKeys_<TypeList<CollectKeys...>, TypeDict<>> {
    using type = TypeList<CollectKeys...>;
};

template<typename ...CollectKeys, typename K, typename V, typename ...Remain>
struct TypeDictAllKeys_<TypeList<CollectKeys...>, TypeDict<PairType<K, V>, Remain...>> {
    using type = typename TypeDictAllKeys_<TypeList<CollectKeys..., K>, TypeDict<Remain...>>::type;
};

template<typename Dict>
using TypeDictAllKeys = typename TypeDictAllKeys_<TypeList<>, Dict>::type;


#pragma mark - TypeDictAllValues
template<typename ...T>
struct TypeDictAllValues_;

template<typename ...CollectValues>
struct TypeDictAllValues_<TypeList<CollectValues...>, TypeDict<>> {
    using type = TypeList<CollectValues...>;
};

template<typename ...CollectValues, typename K, typename V, typename ...Remain>
struct TypeDictAllValues_<TypeList<CollectValues...>, TypeDict<PairType<K, V>, Remain...>> {
    using type = typename TypeDictAllValues_<TypeList<CollectValues..., V>, TypeDict<Remain...>>::type;
};

template<typename Dict>
using TypeDictAllValues = typename TypeDictAllValues_<TypeList<>, Dict>::type;

}

}

#endif //XLIVE_TEMPLATE_HPP

