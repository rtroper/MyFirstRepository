using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Npgsql;

namespace PostgreSQLDataExchangeTest
{
    static class Constants
    {
        // GoldSim reference date corresponds to Julian date 0 days in GoldSim
        public static readonly DateTime GSReferenceDate = new DateTime(1899, 12, 30);
    }

    class Program
    {
        static void Main(string[] args)
        {
            var connString = "Host=localhost;Username=postgres;Password=password;Database=sandbox";

            Console.WriteLine(Constants.GSReferenceDate);

            using (var conn = new NpgsqlConnection(connString))
            {
                conn.Open();

                // Retrieve all rows
                using (var cmd = new NpgsqlCommand("SELECT * from streamflow_data WHERE date >= '10/1/2010'", conn))
                using (var reader = cmd.ExecuteReader())
                {
                    DateTime current;
                    while (reader.Read())
                    {
                        current = NpgsqlTypes.NpgsqlDate.ToDateTime(reader.GetDate(0));
                        Console.Write("{0}\t{1}\t{2}\n", current, current.Subtract(Constants.GSReferenceDate).TotalSeconds, reader[1]);
                    }
                }
            }

            Console.WriteLine("---- END OF DATA ----");
        }
    }
}
