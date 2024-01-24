#include <gtest/gtest.h>
#include<iostream>
using namespace std;

//класс для конструкции базы данных
class DATABase
{
public:
    DATABase()
    {}
    //добавляем необходимые нам методы:открытие базы данных
    void openDB()
    {}
    //добавляем необходимые нам методы: закрытие БД
    void closeDB()
    {}
    //добавляем необходимые нам методы:запрос ДБ
    void queryDB() {}
};

//класс гипотетических команд
class SomeProtocolCommand
{};

//класс ответа
class SomeProtocolResponce
{
public:
    //добавляем необходимые нам методы   
    bool operator==(const SomeProtocolResponce) const
    {
        return false;
    }
};

class SomeExchangeChanelInterface
{
public:
    SomeExchangeChanelInterface()
    {}

    virtual ~SomeExchangeChanelInterface()
    {}


    //метод для записи данных в БД
    virtual size_t writeData(const DATABase& data) = 0;
    //метод для чтения данных из БД
    virtual DATABase readData() = 0;


    //метод ожидания того, что данные записаны в БД
    virtual bool waitForDataWriten(int ms) = 0;
    //метод ожидания, что появились данные, которые можно  из БД
    virtual bool waitForReadyRead(int ms) = 0;
};


//теперь реализуем наш класс канала на основе интерфейса
//пока везде реализация методов пустая, но это только начало :)
class SomeExchangeChanel : public SomeExchangeChanelInterface
{
public:
    SomeExchangeChanel()
    {}

    virtual ~SomeExchangeChanel()
    {}

    virtual size_t writeData(const DATABase& data) override
    {
        return 0;
    }

    virtual DATABase readData() override
    {
        return DATABase();
    }

    virtual bool waitForDataWriten(int ms) override
    {
        return true;
    }

    virtual bool waitForReadyRead(int ms) override
    {
        return true;
    }
};

class SomeProtocolInterface
{
public:
    SomeProtocolInterface()
    {}

    virtual ~SomeProtocolInterface()
    {}

    
    virtual DATABase cmd2raw(const SomeProtocolCommand& cmd) const = 0;

    
    virtual SomeProtocolResponce data2Responce(const DATABase& data) const = 0;

    
    virtual SomeProtocolResponce makeExchange(SomeExchangeChanelInterface* chanel, const SomeProtocolCommand& cmd) = 0;
};



class SomeRealProtocol : public SomeProtocolInterface
{
public:
    SomeRealProtocol()
    {}

    virtual ~SomeRealProtocol()
    {}

    
    virtual DATABase cmd2raw(const SomeProtocolCommand& cmd) const override
    {
        return DATABase();
    }

    
    virtual SomeProtocolResponce data2Responce(const DATABase& data) const override
    {
        return SomeProtocolResponce();
    }

    virtual SomeProtocolResponce makeExchange(SomeExchangeChanelInterface* chanel, const SomeProtocolCommand& cmd)
    {
        return SomeProtocolResponce();
    }

};


//создаем класс мок-объекта канала
class MockExchangeChanel : public SomeExchangeChanelInterface
{
public:
    MOCK_METHOD(size_t, writeData, (const DATABase& data), (override));
    MOCK_METHOD(DATABase, readData, (), (override));

    MOCK_METHOD(bool, waitForDataWriten, (int ms), (override));
    MOCK_METHOD(bool, waitForReadyRead, (int ms), (override));
};



class MockProtocol : public SomeProtocolInterface
{
public:
    MOCK_METHOD(DATABase, cmd2raw, (const SomeProtocolCommand& cmd), (const, override));
    MOCK_METHOD(SomeProtocolResponce, data2Responce, (const DATABase& data), (const, override));
    MOCK_METHOD(SomeProtocolResponce, makeExchange, (SomeExchangeChanelInterface* chanel, const SomeProtocolCommand& cmd), (override));
};



class SomeTestSuite : public ::testing::Test
{
protected:
    void SetUp()
    {
        someProtocol = new SomeRealProtocol();
    }

    void TearDown()
    {
        delete someProtocol;
    }

protected:
    SomeProtocolInterface* someProtocol;
};



TEST_F(SomeTestSuite, testcase1)
{
    SomeProtocolCommand cmd;
    DATABase data = someProtocol->cmd2raw(cmd);
    DATABase reference("\x21\x21\x32\x12\x12");
    ASSERT_EQ(data, reference);
}



TEST_F(SomeTestSuite, testcase2)
{
    SomeProtocolResponce reference;
    DATABase responceData("\x21\x21\x32\x12\x12");
    SomeProtocolResponce responce = someProtocol->data2Responce(responceData);
    ASSERT_EQ(reference, responce);
}



TEST_F(SomeTestSuite, testcase3)
{
    SomeProtocolCommand cmd;
    SomeProtocolResponce reference;


    
    DATABase outData("\x21\x21\x32\x12\x12");
    DATABase inData("\x21\x21\x32\x12\x12");


    //создаем мок-объект
    MockExchangeChanel mchanel;

    
    EXPECT_CALL(mchanel, writeData).WillOnce(::testing::Return(outData.size()));
    EXPECT_CALL(mchanel, readData).WillOnce(::testing::Return(inData));


    
    EXPECT_CALL(mchanel, waitForDataWriten(50)).WillOnce(::testing::Return(true));
    EXPECT_CALL(mchanel, waitForReadyRead(150)).WillRepeatedly(::testing::Return(true));

    SomeProtocolResponce responce = someProtocol->makeExchange(&mchanel, cmd);
    ASSERT_EQ(reference, responce);
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}